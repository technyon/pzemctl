//
// Created by daphreak on 07.03.21.
//

#include "Network.h"
#include <Arduino.h>
#include <src/Arduino_FreeRTOS.h>
#include "Led.h"

Network::Network(Configuration* configuration)
: _configuration(configuration)
{
    _mac[0] = 0x00;
    _mac[1] = 0x08;
    _mac[2] = 0xDC;
    _mac[3] = 0x2A;
    _mac[4] = 0xAC;
    _mac[5] = 0x5F;
}

Network::~Network()
{
    delete _mqttClient;
    _mqttClient = nullptr;
    delete _ethClient;
    _ethClient = nullptr;
}

void Network::initialize()
{
    _ethClient = new EthernetClient();

    Ethernet.init(ETHERNET_CS_PIN);

    IPAddress server;
    server.fromString(_configuration->mqttServerAddress);

    _mqttClient = new PubSubClient(*_ethClient);
    _mqttClient->setServer(server, 1883);

    _mqttClient->setCallback(onMqttDataReceived);

    _fromTask = true;
}


void Network::initializeEthernet()
{
    // start the Ethernet connection:
    Serial.println(F("Initialize Ethernet with DHCP:"));

    int dhcpRetryCnt = 0;

    hw::Led::setNetworkLed(0);

    while(dhcpRetryCnt < 3)
    {
        Serial.print(F("DHCP connect try #"));
        Serial.print(dhcpRetryCnt);
        Serial.println();
        dhcpRetryCnt++;

        if (Ethernet.begin(_mac, 1000, 1000) == 0)
        {
            Serial.println(F("Failed to configure Ethernet using DHCP"));
            // Check for Ethernet hardware present
            if (Ethernet.hardwareStatus() == EthernetNoHardware)
            {
                Serial.println(F("Ethernet shield was not found.  Sorry, can't run without hardware. :("));
                return;
            }
            if (Ethernet.linkStatus() == LinkOFF)
            {
                Serial.println(F("Ethernet cable is not connected."));
            }

            IPAddress ip;
            ip.fromString(_configuration->ipAddress);

            IPAddress subnet;
            subnet.fromString(_configuration->subnetMask);

            IPAddress dns;
            dns.fromString(_configuration->dnsServerAddress);

            IPAddress gateway;
            gateway.fromString(_configuration->gatewayAddress);

            // try to congifure using IP address instead of DHCP:
            Ethernet.begin(_mac, ip, dns);
            Ethernet.setSubnetMask(subnet);
            Ethernet.setGatewayIP(gateway);

            nwDelay(2000);
        }
        else
        {
            hw::Led::setNetworkLed(100);

            dhcpRetryCnt = 1000;
            Serial.print(F("  DHCP assigned IP "));
            Serial.println(Ethernet.localIP());
        }
    }
}

void Network::ethernetHardwareReset()
{
    pinMode(ETHERNET_RESET_PIN, OUTPUT);
    digitalWrite(ETHERNET_RESET_PIN, HIGH);
    nwDelay(250);
    digitalWrite(ETHERNET_RESET_PIN, LOW);
    nwDelay(50);
    digitalWrite(ETHERNET_RESET_PIN, HIGH);
    nwDelay(1500);
}

void Network::reconnect()
{
    while (!_mqttClient->connected())
    {
        ethernetHardwareReset();
        initializeEthernet();

        Serial.print(F("Attempting MQTT connection..."));

        if (_mqttClient->connect("Pzem004t"))
        {
            Serial.println(F("connected"));
            hw::Led::setNetworkLed(255);
            _mqttClient->publish(ledBrightness, "0\0x00");
            _mqttClient->subscribe(ledBrightness);
        }
        else
        {
            Serial.print(F("failed, rc="));
            Serial.print(_mqttClient->state());
            Serial.println(F(" try again in 5 seconds"));
            hw::Led::setNetworkLed(0);
            nwDelay(5000);
        }
    }
}

void Network::update(const hw::pzem004tvalues& phase1, const hw::pzem004tvalues& phase2, const hw::pzem004tvalues& phase3)
{
    int ledVal = 0;

    _updateCnt++;

    if(Ethernet.linkStatus() == EthernetLinkStatus::LinkON)
    {
        ledVal = ledVal + 128;
    }

    if(_mqttClient->state() == 0)
    {
        ledVal = ledVal + 127;
    }

    hw::Led::setNetworkLed(ledVal);

    if(_updateCnt > 10)
    {
        if(Ethernet.maintain() != 0)
        {
            reconnect();
        }
    }

    if (!_mqttClient->connected())
    {
        reconnect();
    }
    _mqttClient->loop();

    if(_updateCnt > 10)
    {
//        Serial.println(_mqttClient->state());

        _updateCnt = 0;

        publishFloat(phase1Voltage, phase1.voltage, 2);
        publishFloat(phase1Current, phase1.current, 2);
        publishFloat(phase1Energy, phase1.energy, 2);
        publishFloat(phase1Frequency, phase1.frequency, 2);
        publishFloat(phase1PowerFactor, phase1.pf, 2);

        publishFloat(phase2Voltage, phase2.voltage, 2);
        publishFloat(phase2Current, phase2.current, 2);
        publishFloat(phase2Energy, phase2.energy, 2);
        publishFloat(phase2Frequency, phase2.frequency, 2);
        publishFloat(phase2PowerFactor, phase2.pf, 2);

        publishFloat(phase3Voltage, phase3.voltage, 2);
        publishFloat(phase3Current, phase3.current, 2);
        publishFloat(phase3Energy, phase3.energy, 2);
        publishFloat(phase3Frequency, phase3.frequency, 2);
        publishFloat(phase3PowerFactor, phase3.pf, 2);
    }
}

void Network::onMqttDataReceived(char* topic, byte* payload, unsigned int length)
{
    hw::Led::setBrightnessWhite((int) atof((char *) payload) * 2.55);
}

void Network::publishFloat(const char* topic, const float &value, const float& precision)
{
    if(_mqttClient->state() != 0)
    {
        Serial.println(F("MQTT not connected, abort publish."));
        return;
    }

    dtostrf(value, 20, precision, _charVal);

    _charIndex = 0;
    while(_charVal[_charIndex] == _space)
    {
        _charIndex++;
    }

    _mqttClient->publish(topic, _charVal + _charIndex);
}

void Network::nwDelay(unsigned long ms)
{
    if(_fromTask)
    {
        vTaskDelay( ms / portTICK_PERIOD_MS);
    }
    else
    {
        delay(ms);
    }
}

EthernetClient *Network::ethernetClient()
{
    return _ethClient;
}

void Network::configurationChanged()
{
    reconnect();
}
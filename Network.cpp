//
// Created by daphreak on 07.03.21.
//

#include "Network.h"
#include <Arduino.h>
#include <src/Arduino_FreeRTOS.h>
#include "Led.h"



Network* nwInst;

Network::Network(Configuration* configuration, void (*viewChangedCallback)(int value), void (*phaseChangedCallback)(int value))
: _configuration(configuration),
  _viewChangedCallback(viewChangedCallback),
  _phaseChangedCallback(phaseChangedCallback)
{
    nwInst = this;
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

    _mqttClient = new PubSubClient(*_ethClient);
//    _mqttClient->setCallback(onMqttDataReceivedCallback);

    _mqttClient->setCallback(Network::onMqttDataReceivedCallback);

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

        if (Ethernet.begin(_configuration->mac, 1000, 1000) == 0)
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
            Ethernet.begin(_configuration->mac, ip, dns);
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
    IPAddress server;
    server.fromString(_configuration->mqttServerAddress);
    _mqttClient->setServer(server, 1883);

    Serial.print(F("Connecting to server: "));
    Serial.println(server);

    while (!_mqttClient->connected() && !_configMode)
    {
        ethernetHardwareReset();
        initializeEthernet();

        Serial.print(F("Attempting MQTT connection..."));

        if (_mqttClient->connect("pzem004t"))
        {
            Serial.println(F("connected"));
            hw::Led::setNetworkLed(255);
            _mqttClient->publish(_led1Brightness, "0");
            _mqttClient->subscribe(_led1Brightness);
            _mqttClient->publish(_led2Brightness, "0");
            _mqttClient->subscribe(_led2Brightness);

            _mqttClient->subscribe(_selectedView);
            _mqttClient->subscribe(_selectedPhase);

            publishView(0);
            publishPhase(0);
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
    if(_configMode) return;
    _updating = true;

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

    if(_viewChanged)
    {
        _viewChanged = false;
        char cstr[5];
        itoa(_currentView, cstr, 5);
        _mqttClient->publish(_selectedView, cstr);
    }

    if(_phaseChanged)
    {
        _phaseChanged = false;
        char cstr[5];
        itoa(_currentPhase, cstr, 5);
        _mqttClient->publish(_selectedPhase, cstr);
    }

    _updating = false;
}

void Network::onMqttDataReceivedCallback(char* topic, byte* payload, unsigned int length)
{
    nwInst->onMqttDataReceived(topic, payload, length);
}


void Network::onMqttDataReceived(char *&topic, byte *&payload, unsigned int &length)
{
    if(strcmp(topic, _led1Brightness) == 0)
    {
        hw::Led::setBrightnessWhite((int) atof((char *) payload) * 2.55);
    }
    if(strcmp(topic, _led2Brightness) == 0)
    {
        hw::Led::setBrightnessBlue((int) atof((char *) payload) * 2.55);
    }
    if(strcmp(topic, _selectedView) == 0)
    {
        Network::_viewChangedCallback((int) atof((char *) payload));
    }
    if(strcmp(topic, _selectedPhase) == 0)
    {
        Network::_phaseChangedCallback((int) atof((char *) payload));
    }
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

void Network::enableConfigMode()
{
    _configMode = true;

    while(_updating)
    {
        vTaskDelay( 200 / portTICK_PERIOD_MS);
    }

    ethernetHardwareReset();
    initializeEthernet();
}

void Network::disableConfigMode()
{
    _configMode = false;
}

const IPAddress Network::ipAddress()
{
    return Ethernet.localIP();
}

void Network::publishView(int value)
{
    _currentView = value;
    _viewChanged = true;
}

void Network::publishPhase(int value)
{
    _currentPhase = value;
    _phaseChanged = true;
}

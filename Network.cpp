//
// Created by daphreak on 07.03.21.
//

#include "Network.h"
#include <Arduino.h>
#include <src/Arduino_FreeRTOS.h>
#include "Led.h"



Network* nwInst;

Network::Network(Configuration* configuration,
                 void (*networkEventCallback)(const NetworkEvent& value))
: _configuration(configuration),
  _networkEventCallback(networkEventCallback)
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
                Serial.println(F("Ethernet module not found"));
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

            if(_firstConnect)
            {
                _mqttClient->subscribe(_led2BrightnessTopic);
                _mqttClient->subscribe(_led1BrightnessTopic);
                _mqttClient->subscribe(_selectedViewTopic);
                _mqttClient->subscribe(_selectedPhaseTopic);
                _mqttClient->subscribe(_switchStateTopic);
                _mqttClient->subscribe(_resetEnergyTopic);
                _mqttClient->subscribe(_customViewValue1Topic);
                _mqttClient->subscribe(_customViewValue2Topic);
                _mqttClient->subscribe(_customViewValue3Topic);

                _firstConnect = false;
                _mqttClient->publish(_led1BrightnessTopic, "0");
                _mqttClient->publish(_led2BrightnessTopic, "0");

                publishView(_currentView);
                publishPhase(_currentPhase);
                publishSwitchState(_switchState);
                _mqttClient->publish(_resetEnergyTopic, "");
                _mqttClient->publish(_customViewValue1Topic, "0");
                _mqttClient->publish(_customViewValue2Topic, "1");
                _mqttClient->publish(_customViewValue3Topic, "5");
            }
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
    long ts = millis();

    if(_configMode) return;
    _updating = true;

    int ledVal = 0;

    if(Ethernet.linkStatus() == EthernetLinkStatus::LinkON)
    {
        ledVal = ledVal + 128;
    }

    if(_mqttClient->state() == 0)
    {
        ledVal = ledVal + 127;
    }

    hw::Led::setNetworkLed(ledVal);

    if((ts - _lastMaintain) > 1000)
    {
        _lastMaintain = ts;
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

    if((ts - _lastPublish) > _configuration->numericMqttPublishInterval())
    {
        _lastPublish = ts;

        publishFloat(phase1VoltageTopic, phase1.voltage, 2);
        publishFloat(phase1CurrentTopic, phase1.current, 2);
        publishFloat(phase1EnergyTopic, phase1.energy, 2);
        publishFloat(phase1FrequencyTopic, phase1.frequency, 2);
        publishFloat(phase1PowerFactorTopic, phase1.pf, 2);

        publishFloat(phase2VoltageTopic, phase2.voltage, 2);
        publishFloat(phase2CurrentTopic, phase2.current, 2);
        publishFloat(phase2EnergyTopic, phase2.energy, 2);
        publishFloat(phase2FrequencyTopic, phase2.frequency, 2);
        publishFloat(phase2PowerFactorTopic, phase2.pf, 2);

        publishFloat(phase3VoltageTopic, phase3.voltage, 2);
        publishFloat(phase3CurrentTopic, phase3.current, 2);
        publishFloat(phase3EnergyTopic, phase3.energy, 2);
        publishFloat(phase3FrequencyTopic, phase3.frequency, 2);
        publishFloat(phase3PowerFactorTopic, phase3.pf, 2);
    }

    if(_viewChanged)
    {
        _viewChanged = false;
        char cstr[5];
        itoa(_currentView, cstr, 5);
        _mqttClient->publish(_selectedViewTopic, cstr);
    }

    if(_phaseChanged)
    {
        _phaseChanged = false;
        char cstr[5];
        itoa(_currentPhase, cstr, 5);
        _mqttClient->publish(_selectedPhaseTopic, cstr);
    }

    if(_switchStateChanged)
    {
        _switchStateChanged = false;
        if(_switchState)
        {
            _mqttClient->publish(_switchStateTopic, "1");
        }
        else
        {
            _mqttClient->publish(_switchStateTopic, "0");
        }
    }

    _updating = false;
}

void Network::onMqttDataReceivedCallback(char* topic, byte* payload, unsigned int length)
{
    nwInst->onMqttDataReceived(topic, payload, length);
}


void Network::onMqttDataReceived(char*& topic, byte*& payload, unsigned int& length)
{
    char value[10];
    size_t l = min(length, sizeof(value)-1);

    for(int i=0; i<l; i++)
    {
        value[i] = payload[i];
    }

    value[l] = 0;
/*
    Serial.print(F("MQTT Message: "));
    Serial.print(topic);
    Serial.print(F(" = "));
    Serial.println(value);
*/
    if(strcmp(topic, _led1BrightnessTopic) == 0)
    {
        hw::Led::setBrightnessWhite((int) atof(value) * 2.55);
    }
    else if(strcmp(topic, _led2BrightnessTopic) == 0)
    {
        hw::Led::setBrightnessBlue((int) atof(value) * 2.55);
    }
    else if(strcmp(topic, _selectedViewTopic) == 0)
    {
        NetworkEvent event
                {
                    type: NetworkEventType::viewChange,
                    paramInt: atoi(value)
                };
        _networkEventCallback(event);
    }
    else if(strcmp(topic, _selectedPhaseTopic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::phaseChange,
                        paramInt: atoi(value)
                };
        _networkEventCallback(event);
    }
    else if(strcmp(topic, _switchStateTopic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::switchStateChange,
                };
        event.paramBool = atoi(value) > 0;
        _networkEventCallback(event);
    }
    else if(strcmp(topic, _resetEnergyTopic) == 0)
    {
        if(strcmp(value, "confirm") == 0)
        {
            NetworkEvent event
                    {
                            type: NetworkEventType::resetEnergy
                    };
            _networkEventCallback(event);
        }
        _mqttClient->publish(_resetEnergyTopic, "");
    }
    else if(strcmp(topic, _customViewValue1Topic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::customViewValue1Changed,
                        paramInt: atoi(value)
                };
        _networkEventCallback(event);
    }
    else if(strcmp(topic, _customViewValue2Topic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::customViewValue2Changed,
                        paramInt: atoi(value)
                };
        _networkEventCallback(event);
    }
    else if(strcmp(topic, _customViewValue3Topic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::customViewValue3Changed,
                        paramInt: atoi(value)
                };
        _networkEventCallback(event);
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

void Network::publishSwitchState(bool value)
{
    _switchState = value;
    _switchStateChanged = true;
}

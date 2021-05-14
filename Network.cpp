//
// Created by daphreak on 07.03.21.
//

#include "Network.h"
#include <Arduino.h>
#include <src/Arduino_FreeRTOS.h>
#include "Led.h"



Network* nwInst;

Network::Network(EepromConfiguration* configuration,
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

    _mqttClient->unsubscribe(led2BrightnessTopic);
    _mqttClient->unsubscribe(led1BrightnessTopic);
    _mqttClient->unsubscribe(selectedViewTopic);
    _mqttClient->unsubscribe(selectedPhaseTopic);
    _mqttClient->unsubscribe(switchStateTopic);
    _mqttClient->unsubscribe(resetEnergyTopic);
    _mqttClient->unsubscribe(customViewValue1Topic);
    _mqttClient->unsubscribe(customViewValue2Topic);
    _mqttClient->unsubscribe(customViewValue3Topic);
    _mqttClient->unsubscribe(phase1EnergyOffsetTopic);
    _mqttClient->unsubscribe(phase2EnergyOffsetTopic);
    _mqttClient->unsubscribe(phase3EnergyOffsetTopic);

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

            _mqttClient->subscribe(led2BrightnessTopic);
            _mqttClient->subscribe(led1BrightnessTopic);
            _mqttClient->subscribe(selectedViewTopic);
            _mqttClient->subscribe(selectedPhaseTopic);
            _mqttClient->subscribe(switchStateTopic);
            _mqttClient->subscribe(resetEnergyTopic);
            _mqttClient->subscribe(customViewValue1Topic);
            _mqttClient->subscribe(customViewValue2Topic);
            _mqttClient->subscribe(customViewValue3Topic);
            _mqttClient->subscribe(phase1EnergyOffsetTopic);
            _mqttClient->subscribe(phase2EnergyOffsetTopic);
            _mqttClient->subscribe(phase3EnergyOffsetTopic);

            if(_firstConnect)
            {
                _firstConnect = false;
                _mqttClient->publish(led1BrightnessTopic, "0");
                _mqttClient->publish(led2BrightnessTopic, "0");

                publishView(_currentView);
                publishPhase(_currentPhase);
                publishSwitchState(_switchState);
                _mqttClient->publish(resetEnergyTopic, "");
                _mqttClient->publish(customViewValue1Topic, "0");
                _mqttClient->publish(customViewValue2Topic, "1");
                _mqttClient->publish(customViewValue3Topic, "5");
                _mqttClient->publish(phase1EnergyOffsetTopic, "0");
                _mqttClient->publish(phase2EnergyOffsetTopic, "0");
                _mqttClient->publish(phase3EnergyOffsetTopic, "0");
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

    _reconnectCount++;
    char reconnectStr[12];
    itoa(_reconnectCount, reconnectStr, 10);
    _mqttClient->publish(reconnectCountTopic, reconnectStr);
}

void Network::update(const hw::pzem004tvalues& phasesCombined, const hw::pzem004tvalues& phase1, const hw::pzem004tvalues& phase2, const hw::pzem004tvalues& phase3)
{
    long ts = millis();

    if(_configMode) return;
    _updating = true;

    uint8_t ledVal = 0;

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

    if(_reconnectRequested)
    {
        _reconnectRequested = false;
        reconnect();
    }
    if (!_mqttClient->connected())
    {
        reconnect();
    }
    _mqttClient->loop();

    if((ts - _lastPublish) > _configuration->numericMqttPublishInterval())
    {
        _lastPublish = ts;
//        publishFloat(phasesCombinedVoltageTopic, phasesCombined.voltage, 2);
//        publishFloat(phasesCombinedCurrentTopic, phasesCombined.current, 2);
//        publishFloat(phasesCombinedPowerTopic, phasesCombined.power, 2);
//        publishFloat(phasesCombinedEnergyTopic, phasesCombined.energy, 2);
//        publishFloat(phasesCombinedFrequencyTopic, phasesCombined.frequency, 2);
//        publishFloat(phasesCombinedPowerFactorTopic, phasesCombined.pf, 2);

        publishFloat(phase1VoltageTopic, phase1.voltage, 2);
        publishFloat(phase1CurrentTopic, phase1.current, 2);
        publishFloat(phase1PowerTopic, phase1.power, 2);
        publishFloat(phase1EnergyTopic, phase1.energy, 2);
        publishFloat(phase1FrequencyTopic, phase1.frequency, 2);
        publishFloat(phase1PowerFactorTopic, phase1.pf, 2);

        publishFloat(phase2VoltageTopic, phase2.voltage, 2);
        publishFloat(phase2CurrentTopic, phase2.current, 2);
        publishFloat(phase2PowerTopic, phase2.power, 2);
        publishFloat(phase2EnergyTopic, phase2.energy, 2);
        publishFloat(phase2FrequencyTopic, phase2.frequency, 2);
        publishFloat(phase2PowerFactorTopic, phase2.pf, 2);

        publishFloat(phase3VoltageTopic, phase3.voltage, 2);
        publishFloat(phase3CurrentTopic, phase3.current, 2);
        publishFloat(phase3PowerTopic, phase3.power, 2);
        publishFloat(phase3EnergyTopic, phase3.energy, 2);
        publishFloat(phase3FrequencyTopic, phase3.frequency, 2);
        publishFloat(phase3PowerFactorTopic, phase3.pf, 2);
    }

    if(_viewChanged)
    {
        _viewChanged = false;
        char cstr[2];
        itoa(_currentView, cstr, 10);
        _mqttClient->publish(selectedViewTopic, cstr);
    }

    if(_phaseChanged)
    {
        _phaseChanged = false;
        char cstr[2];
        itoa(_currentPhase, cstr, 10);
        _mqttClient->publish(selectedPhaseTopic, cstr);
    }

    if(_switchStateChanged)
    {
        _switchStateChanged = false;
        if(_switchState)
        {
            _mqttClient->publish(switchStateTopic, "1");
        }
        else
        {
            _mqttClient->publish(switchStateTopic, "0");
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
    if(strcmp(topic, led1BrightnessTopic) == 0)
    {
        hw::Led::setBrightnessWhite((int) atof(value) * 2.55);
    }
    else if(strcmp(topic, led2BrightnessTopic) == 0)
    {
        hw::Led::setBrightnessBlue((int) atof(value) * 2.55);
    }
    else if(strcmp(topic, selectedViewTopic) == 0)
    {
        NetworkEvent event
                {
                    type: NetworkEventType::viewChange,
                    paramInt: atoi(value)
                };
        _networkEventCallback(event);
    }
    else if(strcmp(topic, selectedPhaseTopic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::phaseChange,
                        paramInt: atoi(value)
                };
        _networkEventCallback(event);
    }
    else if(strcmp(topic, switchStateTopic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::switchStateChange,
                };
        event.paramBool = atoi(value) > 0;
        _networkEventCallback(event);
    }
    else if(strcmp(topic, resetEnergyTopic) == 0)
    {
        if(strcmp(value, "confirm") == 0)
        {
            NetworkEvent event
                    {
                            type: NetworkEventType::resetEnergy
                    };
            _networkEventCallback(event);
        }
        _mqttClient->publish(resetEnergyTopic, "");
    }
    else if(strcmp(topic, customViewValue1Topic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::customViewValue1Changed,
                        paramInt: atoi(value)
                };
        _networkEventCallback(event);
    }
    else if(strcmp(topic, customViewValue2Topic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::customViewValue2Changed,
                        paramInt: atoi(value)
                };
        _networkEventCallback(event);
    }
    else if(strcmp(topic, customViewValue3Topic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::customViewValue3Changed,
                        paramInt: atoi(value)
                };
        _networkEventCallback(event);
    }
    else if(strcmp(topic, phase1EnergyOffsetTopic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::phase1OffsetChanged,
                };
        event.paramFloat = atof(value);
        _networkEventCallback(event);
    }
    else if(strcmp(topic, phase2EnergyOffsetTopic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::phase2OffsetChanged,
                };
        event.paramFloat = atof(value);
        _networkEventCallback(event);
    }
    else if(strcmp(topic, phase3EnergyOffsetTopic) == 0)
    {
        NetworkEvent event
                {
                        type: NetworkEventType::phase3OffsetChanged,
                };
        event.paramFloat = atof(value);
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
    _reconnectRequested = true;
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

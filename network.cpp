//
// Created by daphreak on 07.03.21.
//

#include "network.h"
#include <Arduino.h>

network::network()
: _dns(192, 168, 0, 101),
  _ip(192, 168, 0, 36),
  _server(192, 168, 0, 100)
{
    _mac[0] = 0xDE;
    _mac[1] = 0xAD;
    _mac[2] = 0xBE;
    _mac[3] = 0xEF;
    _mac[4] = 0xFE;
    _mac[5] = 0xED;
}

network::~network()
{
    delete _mqttClient;
    _mqttClient = nullptr;
    delete _ethClient;
    _ethClient = nullptr;
}

void network::initialize()
{
    _ethClient = new EthernetClient();

    Ethernet.init(ETHERNET_CS_PIN);
    ethernetHardwareReset(ETHERNET_RESET_PIN);

    // start the Ethernet connection:
    Serial.println("Initialize Ethernet with DHCP:");
    if (Ethernet.begin(_mac, 1000, 1000) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        // Check for Ethernet hardware present
        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
            Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
            while (true) {
                delay(1); // do nothing, no point running without Ethernet hardware
            }
        }
        if (Ethernet.linkStatus() == LinkOFF) {
            Serial.println("Ethernet cable is not connected.");
        }
        // try to congifure using IP address instead of DHCP:
        Ethernet.begin(_mac, _ip, _dns);
    } else {
        Serial.print("  DHCP assigned IP ");
        Serial.println(Ethernet.localIP());
    }

    _mqttClient = new PubSubClient(*_ethClient);
    _mqttClient->setServer(_server, 1883);
    _mqttClient->setCallback(callback);

    // Allow the hardware to sort itself out
//    delay(1500);
}

void network::ethernetHardwareReset(const uint8_t resetPin)
{
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);
    delay(250);
    digitalWrite(resetPin, LOW);
    delay(50);
    digitalWrite(resetPin, HIGH);
    delay(1000);
}
unsigned long byteCount = 0;

void network::update()
{
    if (!_mqttClient->connected())
    {
        reconnect();
    }
    _mqttClient->loop();
}

void network::callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i=0;i<length;i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}


void network::reconnect()
{
    while (!_mqttClient->connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (_mqttClient->connect("arduinoClient"))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            _mqttClient->publish("outTopic","hello world");
            // ... and resubscribe
            _mqttClient->subscribe("inTopic");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(_mqttClient->state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}
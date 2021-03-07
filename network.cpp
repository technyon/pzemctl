//
// Created by daphreak on 07.03.21.
//

#include "network.h"
#include <Arduino.h>

network::network()
: _ip(192, 168, 0, 36),
  _server(192, 168, 0, 100),
  _mqttClient(_ethClient)
{
    _mac[0] = 0xD1;
    _mac[1] = 0xE4;
    _mac[2] = 0x23;
    _mac[3] = 0x5F;
    _mac[4] = 0xA8;
    _mac[5] = 0xC3;
}

void network::initialize()
{
    Ethernet.init(53);
    Ethernet.begin(_mac, _ip);

    _mqttClient.setServer(_server, 1883);
    _mqttClient.setCallback(callback);

    // Allow the hardware to sort itself out
    delay(1500);
}

void network::update()
{
    if (!_mqttClient.connected()) {
        reconnect();
    }
    _mqttClient.loop();
}

void network::callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i=0;i<length;i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void network::reconnect()
{
    while (!_mqttClient.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (_mqttClient.connect("arduinoClient")) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            _mqttClient.publish("outTopic","hello world");
            // ... and resubscribe
            _mqttClient.subscribe("inTopic");
        } else {
            Serial.print("failed, rc=");
            Serial.print(_mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

#pragma once

#include <Ethernet.h>
#include <PubSubClient.h>
#include "pzem004t.h"

class network
{
private:
    static const uint8_t ETHERNET_CS_PIN = 53;
    static const uint8_t ETHERNET_RESET_PIN = 49;

public:
    explicit network();
    virtual ~network();

    void initialize();
    void update();
    void publish(const hw::pzem004tvalues& phase1, const hw::pzem004tvalues& phase2, const hw::pzem004tvalues& phase3);

private:
    void ethernetHardwareReset(const uint8_t resetPin);
    void publishFloat(const char* topic, const float& value, const float& precision);

    static void callback(char* topic, byte* payload, unsigned int length);
    void reconnect();

    IPAddress _dns;

    byte _mac[6];
    IPAddress _ip;
    IPAddress _server;

    char charVal[20];

    EthernetClient* _ethClient;
    PubSubClient* _mqttClient;
};

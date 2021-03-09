#pragma once

#include <Ethernet.h>
#include <PubSubClient.h>

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

private:
    void ethernetHardwareReset(const uint8_t resetPin);

    static void callback(char* topic, byte* payload, unsigned int length);
    void reconnect();

    IPAddress _dns;


    byte _mac[6];
    IPAddress _ip;
    IPAddress _server;


    EthernetClient* _ethClient;
    PubSubClient* _mqttClient;
};

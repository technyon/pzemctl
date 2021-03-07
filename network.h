#pragma once

#include <Ethernet2.h>
#include <PubSubClient.h>

class network
{
public:
    explicit network();
    virtual ~network() = default;

    void initialize();
    void update();

private:
    static void callback(char* topic, byte* payload, unsigned int length);
    void reconnect();

    byte _mac[6];
    IPAddress _ip;
    IPAddress _server;

    EthernetClient _ethClient;
    PubSubClient _mqttClient;
};

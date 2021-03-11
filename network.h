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
    void update(const hw::pzem004tvalues& phase1, const hw::pzem004tvalues& phase2, const hw::pzem004tvalues& phase3);

private:
    const char* topic = "";

    const char* phase1Voltage = "energy/phase1/voltage";
    const char* phase1Current ="energy/phase1/current";
    const char* phase1Energy = "energy/phase1/energy";
    const char* phase1Frequency ="energy/phase1/frequency";
    const char* phase1PowerFactor ="energy/phase1/powerfactor";

    const char* phase2Voltage = "energy/phase2/voltage";
    const char* phase2Current ="energy/phase2/current";
    const char* phase2Energy = "energy/phase2/energy";
    const char* phase2Frequency ="energy/phase2/frequency";
    const char* phase2PowerFactor ="energy/phase2/powerfactor";

    const char* phase3Voltage = "energy/phase3/voltage";
    const char* phase3Current ="energy/phase3/current";
    const char* phase3Energy = "energy/phase3/energy";
    const char* phase3Frequency ="energy/phase3/frequency";
    const char* phase3PowerFactor ="energy/phase3/powerfactor";

    void ethernetHardwareReset(const uint8_t resetPin);
    void publishFloat(const char* topic, const float& value, const float& precision);

    void reconnect();

    IPAddress _dns;

    byte _mac[6];
    IPAddress _ip;
    IPAddress _server;

    const char _space = ' ';
    char _charVal[21];
    int _charIndex;

    uint8_t _updateCnt = 0;

    EthernetClient* _ethClient;
    PubSubClient* _mqttClient;
};

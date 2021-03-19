#pragma once

#include <Ethernet.h>
#include <PubSubClient.h>
#include "Pzem004t.h"
#include "DisplaySSD1306.h"
#include "Configuration.h"

class Network
{
private:
    static const uint8_t ETHERNET_CS_PIN = 53;
    static const uint8_t ETHERNET_RESET_PIN = 49;

public:
    explicit Network(Configuration* configuration);
    virtual ~Network();

    void initialize();
    void update(const hw::pzem004tvalues& phase1, const hw::pzem004tvalues& phase2, const hw::pzem004tvalues& phase3);
    void configurationChanged();

    EthernetClient* ethernetClient();

private:
    void initializeEthernet();
    void ethernetHardwareReset();
    void publishFloat(const char* topic, const float& value, const float& precision);
    static void onMqttDataReceived(char* topic, byte* payload, unsigned int length);

    void reconnect();
    void nwDelay(unsigned long ms);

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

    const char* ledBrightness ="energy/ledBrightness";

    byte _mac[6];

    const char _space = ' ';
    char _charVal[21];
    int _charIndex;
    bool _fromTask = false;

    uint8_t _updateCnt = 0;

    EthernetClient* _ethClient;
    PubSubClient* _mqttClient;

    Configuration* _configuration;
};

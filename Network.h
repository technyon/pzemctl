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
    explicit Network(Configuration* configuration, void (*viewChangedCallback)(int value), void (*_phaseChangedCallback)(int value));
    virtual ~Network();

    void initialize();
    void update(const hw::pzem004tvalues& phase1, const hw::pzem004tvalues& phase2, const hw::pzem004tvalues& phase3);
    void configurationChanged();

    EthernetClient* ethernetClient();

    void enableConfigMode();
    void disableConfigMode();

    const IPAddress ipAddress();

    void publishView(int value);
    void publishPhase(int value);
    void publishSwitchState(bool value);

private:
    void initializeEthernet();
    void ethernetHardwareReset();
    void publishFloat(const char* topic, const float& value, const float& precision);
    static void onMqttDataReceivedCallback(char* topic, byte* payload, unsigned int length);
    void onMqttDataReceived(char*& topic, byte*& payload, unsigned int& length);

    void reconnect();
    void nwDelay(unsigned long ms);

    const char* phase1VoltageTopic = "energy/phase1/voltage";
    const char* phase1CurrentTopic ="energy/phase1/current";
    const char* phase1EnergyTopic = "energy/phase1/energy";
    const char* phase1FrequencyTopic ="energy/phase1/frequency";
    const char* phase1PowerFactorTopic ="energy/phase1/powerfactor";

    const char* phase2VoltageTopic = "energy/phase2/voltage";
    const char* phase2CurrentTopic ="energy/phase2/current";
    const char* phase2EnergyTopic = "energy/phase2/energy";
    const char* phase2FrequencyTopic ="energy/phase2/frequency";
    const char* phase2PowerFactorTopic ="energy/phase2/powerfactor";

    const char* phase3VoltageTopic = "energy/phase3/voltage";
    const char* phase3CurrentTopic ="energy/phase3/current";
    const char* phase3EnergyTopic = "energy/phase3/energy";
    const char* phase3FrequencyTopic ="energy/phase3/frequency";
    const char* phase3PowerFactorTopic ="energy/phase3/powerfactor";

    const char* _led1BrightnessTopic = "energy/led1Brightness";
    const char* _led2BrightnessTopic = "energy/led2Brightness";
    const char* _selectedViewTopic = "energy/view";
    const char* _selectedPhaseTopic = "energy/phase";
    const char* _switchStateTopic = "energy/switchState";

    const char _space = ' ';
    char _charVal[21];
    int _charIndex;
    bool _fromTask = false;

    bool _configMode = false;
    bool _updating = false;

    uint8_t _updateCnt = 0;

    void (*_viewChangedCallback)(int value);
    void (*_phaseChangedCallback)(int value);

    int _currentView = 0;
    int _currentPhase = 0;
    bool _switchState = true;

    bool _viewChanged = false;
    bool _phaseChanged = false;
    bool _switchStateChanged = false;

    EthernetClient* _ethClient;
    PubSubClient* _mqttClient;

    Configuration* _configuration;
};

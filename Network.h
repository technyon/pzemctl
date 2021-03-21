#pragma once

#include <Ethernet.h>
#include <src/PubSubClient.h>
#include "Pzem004t.h"
#include "DisplaySSD1306.h"
#include "Configuration.h"

enum class NetworkEventType
{
    viewChange,
    phaseChange,
    switchStateChange,
    resetEnergy,
    customViewValue1Changed,
    customViewValue2Changed,
    customViewValue3Changed
};

struct NetworkEvent
{
    NetworkEventType type;
    int paramInt;
    bool paramBool;
};

class Network
{
private:
    static const uint8_t ETHERNET_CS_PIN = 53;
    static const uint8_t ETHERNET_RESET_PIN = 49;

public:
    explicit Network(Configuration* configuration,
                     void (*networkEventCallback)(const NetworkEvent& value)
);

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
    const char* _led1BrightnessTopic = "energy/control/led1Brightness";
    const char* _led2BrightnessTopic = "energy/control/led2Brightness";
    const char* _selectedViewTopic = "energy/control/view";
    const char* _selectedPhaseTopic = "energy/control/phase";
    const char* _switchStateTopic = "energy/control/switchState";
    const char* _resetEnergyTopic = "energy/control/resetEnergy";
    const char* _reconnectCountTopic = "energy/control/reconnectCount";
    const char* _customViewValue1Topic = "energy/control/customValue1";
    const char* _customViewValue2Topic = "energy/control/customValue2";
    const char* _customViewValue3Topic = "energy/control/customValue3";


    long _lastPublish = 0;
    long _lastMaintain = 0;
    const char _space = ' ';
    char _charVal[21];
    int _charIndex;
    bool _fromTask = false;
    uint32_t _reconnectCount = 0;

    bool _configMode = false;
    bool _updating = false;
    bool _firstConnect = true;

    void (*_networkEventCallback)(const NetworkEvent& value);

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

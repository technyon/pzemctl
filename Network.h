#pragma once

#include "Config.h"
#include <Ethernet.h>
#include <src/PubSubClient.h>
#include "Pzem004t.h"
#include "DisplaySSD1306.h"
#include "EepromConfiguration.h"

enum class NetworkEventType
{
    viewChange,
    phaseChange,
    switchStateChange,
    resetEnergy,
    customViewValue1Changed,
    customViewValue2Changed,
    customViewValue3Changed,
    phase1OffsetChanged,
    phase2OffsetChanged,
    phase3OffsetChanged
};

struct NetworkEvent
{
    NetworkEventType type;
    int paramInt;
    bool paramBool;
    float paramFloat;
};

class Network
{
private:


public:
    explicit Network(EepromConfiguration* configuration,
                     void (*networkEventCallback)(const NetworkEvent& value)
);

    virtual ~Network();

    void initialize();
    void update(const hw::pzem004tvalues& phasesCombined, const hw::pzem004tvalues& phase1, const hw::pzem004tvalues& phase2, const hw::pzem004tvalues& phase3);
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
    void publishInt(const char* topic, const int& value);
    static void onMqttDataReceivedCallback(char* topic, byte* payload, unsigned int length);
    void onMqttDataReceived(char*& topic, byte*& payload, unsigned int& length);

    void reconnect();
    void nwDelay(unsigned long ms);

    unsigned long _lastPublish = 0;
    unsigned long _lastMaintain = 0;
    const char _space = ' ';
    char _charVal[21];
    int _charIndex;
    bool _fromTask = false;
    bool _reconnectRequested = false;
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

    EepromConfiguration* _configuration;
};

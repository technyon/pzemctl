#pragma once

#define SWITCH_PHASE_PIN 22
#define SWITCH_VIEW_PIN 24
#define SWITCH_ON_OFF_PIN 26

#define LONG_PRESS_THRESHOLD 1000

namespace hw
{
    enum class ButtonId
    {
        SwitchPhase,
        SwitchView,
        SwitchPhaseLong,
        SwitchViewLong,
        OnOff,
        OnOffLong
    };

    class Input
    {
    public:
        explicit Input(void (*buttonPressed)(ButtonId));
        virtual ~Input() = default;

        void initialize();
        void update();

    private:
        void checkSwitchPhase();
        void checkSwitchView();
        void checkOnOff();

        void (*_buttonPressed)(ButtonId);

        int _lastSwitchPhasePinValue = 1;
        int _lastSwitchViewPinValue = 1;
        int _lastOnOffPinValue = 1;

        long switchPhasePressedTs = -1;
        long switchViewPressedTs = -1;
        long switchOnOffPressedTs = -1;
    };
}
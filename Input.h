#pragma once

namespace hw
{
    enum class ButtonId
    {
        SwitchPhase,
        SwitchView,
        SwitchPhaseLong,
        SwitchViewLong,
    };

    class Input
    {
    public:
        explicit Input(void (*buttonPressed)(ButtonId));
        virtual ~Input() = default;

        void initialize();
        void update();

    private:
        const int LONG_PRESS_THRESHOLD = 1000;

        void checkSwitchPhase();
        void checkSwitchView();

        const int SWITCH_PHASE_PIN = 22;
        const int SWITCH_VIEW_PIN = 24;

        void (*_buttonPressed)(ButtonId);

        int _lastSwitchPhasePinValue = 1;
        int _lastSwitchViewPinValue = 1;

        long switchPhasePressedTs = -1;
        long switchViewPressedTs = -1;
    };
}
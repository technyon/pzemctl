#pragma once

namespace hw
{
    enum class ButtonId
    {
        SwitchPhase,
        SwitchView
    };

    class Input
    {
    public:
        explicit Input(void (*buttonPressed)(ButtonId));
        virtual ~Input() = default;

        void initialize();
        void update();

    private:
        const int SWITCH_PHASE_PIN = 22;
        const int SWITCH_VIEW_PIN = 24;

        void (*_buttonPressed)(ButtonId);

        int _lastSwitchPhasePinValue = 1;
        int _lastSwitchViewPinValue = 1;
    };
}
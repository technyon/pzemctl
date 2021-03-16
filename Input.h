#pragma once

namespace hw
{
    enum class ButtonId
    {
        SwitchPhase
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

        void (*_buttonPressed)(ButtonId);

        int _lastSwitchPhasePinValue = 1;
    };
}
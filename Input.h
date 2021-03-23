#pragma once

#include "Config.h"

namespace hw
{
    enum class ButtonId
    {
        SwitchPhase,
        SwitchView,
        SwitchPhaseLong,
        SwitchViewLong,
        SwitchOnOff,
        SwitchOnOffLong
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
        void checkSwitchOnOff();

        void (*_buttonPressed)(ButtonId);

        int _lastSwitchPhasePinValue = 1;
        int _lastSwitchViewPinValue = 1;
        int _lastSwitchOnOffPinValue = 1;

        long switchPhasePressedTs = -1;
        long switchViewPressedTs = -1;
        long switchOnOffPressedTs = -1;
    };
}
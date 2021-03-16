#include "Input.h"
#include <Arduino.h>

namespace hw
{
    Input::Input(void (*buttonPressed)(ButtonId))
    :_buttonPressed(buttonPressed)
    {

    }


    void Input::initialize()
    {
        pinMode(22, INPUT_PULLUP);
    }

    void Input::update()
    {
        int switchPhaseValue = digitalRead(SWITCH_PHASE_PIN);

        if(_lastSwitchPhasePinValue ==1 && switchPhaseValue == 0)
        {
            _buttonPressed(ButtonId::SwitchPhase);
        }

        _lastSwitchPhasePinValue = switchPhaseValue;
    }


}
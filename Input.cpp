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
        pinMode(SWITCH_PHASE_PIN, INPUT_PULLUP);
        pinMode(SWITCH_VIEW_PIN, INPUT_PULLUP);

    }

    void Input::update()
    {
        int value = digitalRead(SWITCH_PHASE_PIN);
        if(_lastSwitchPhasePinValue == 1 && value == 0)
        {
            _buttonPressed(ButtonId::SwitchPhase);
        }
        _lastSwitchPhasePinValue = value;

        value = digitalRead(SWITCH_VIEW_PIN);
        if(_lastSwitchViewPinValue == 1 && value == 0)
        {
            _buttonPressed(ButtonId::SwitchView);
        }
        _lastSwitchViewPinValue = value;

    }


}
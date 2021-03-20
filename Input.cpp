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
        pinMode(SWITCH_ON_OFF_PIN, INPUT_PULLUP);
    }

    void Input::update()
    {
        checkSwitchPhase();
        checkSwitchView();
        checkSwitchOnOff();
    }

    void Input::checkSwitchPhase()
    {
        int value = digitalRead(SWITCH_PHASE_PIN);
        if(_lastSwitchPhasePinValue == 1 && value == 0)
        {
            switchPhasePressedTs = millis();
        }
        if(_lastSwitchPhasePinValue == 0 && value == 1)
        {
            if(millis() - switchPhasePressedTs < LONG_PRESS_THRESHOLD)
            {
                _buttonPressed(ButtonId::SwitchPhase);
            }
            else
            {
                _buttonPressed(ButtonId::SwitchPhaseLong);
            }
        }
        _lastSwitchPhasePinValue = value;
    }

    void Input::checkSwitchView()
    {
        int value = digitalRead(SWITCH_VIEW_PIN);
        if(_lastSwitchViewPinValue == 1 && value == 0)
        {
            switchViewPressedTs = millis();
        }
        if(_lastSwitchViewPinValue == 0 && value == 1)
        {
            if(millis() - switchViewPressedTs < LONG_PRESS_THRESHOLD)
            {
                _buttonPressed(ButtonId::SwitchView);
            }
            else
            {
                _buttonPressed(ButtonId::SwitchViewLong);
            }
        }
        _lastSwitchViewPinValue = value;
    }

    void Input::checkSwitchOnOff()
    {
        int value = digitalRead(SWITCH_ON_OFF_PIN);
        if(_lastSwitchOnOffPinValue == 1 && value == 0)
        {
            switchOnOffPressedTs = millis();
        }
        if(_lastSwitchOnOffPinValue == 0 && value == 1)
        {
            if(millis() - switchOnOffPressedTs < LONG_PRESS_THRESHOLD)
            {
                _buttonPressed(ButtonId::SwitchOnOff);
            }
            else
            {
                _buttonPressed(ButtonId::SwitchOnOffLong);
            }
        }
        _lastSwitchOnOffPinValue = value;
    }


}
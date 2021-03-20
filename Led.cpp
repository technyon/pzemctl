#include <Arduino.h>
#include "Led.h"

namespace hw
{

    void Led::initialize()
    {
        setNetworkLed(0);
        setBrightnessWhite(0);
        setBrightnessBlue(0);
        setBrightnessSwitchState(0);
    }

    void Led::setNetworkLed(const int& value)
    {
        analogWrite(LED3_PIN, constrain(value, 0, 255));
    }

    void Led::setBrightnessWhite(const int &value)
    {
        Serial.println(value);
        analogWrite(LED2_PIN, constrain(value, 0, 255));
    }

    void Led::setBrightnessBlue(const int &value)
    {
        analogWrite(LED1_PIN, constrain(value, 0, 255));
    }

    void Led::setBrightnessSwitchState(const int &value)
    {
        analogWrite(LED4_PIN, constrain(value, 0, 255));
    }
}
#include <Arduino.h>
#include "Led.h"

namespace hw
{

    void Led::initialize()
    {

    }

    void Led::update()
    {

    }

    void Led::setNetworkLed(const int& value)
    {
        analogWrite(LED3_PIN, constrain(value, 0, 255));
    }

    void Led::setBrightnessWhite(const int &value)
    {
        analogWrite(LED2_PIN, constrain(value, 0, 255));
    }

    void Led::setBrightnessBlue(const int &value)
    {
        analogWrite(LED1_PIN, constrain(value, 0, 255));
    }
}
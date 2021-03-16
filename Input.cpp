#include "Input.h"
#include <Arduino.h>

namespace hw
{

    void Input::initialize()
    {
        pinMode(22, INPUT_PULLUP);
    }

    void Input::update()
    {
//        Serial.println(digitalRead(22));
    }
}
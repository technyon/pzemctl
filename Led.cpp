#include <Arduino.h>
#include "Led.h"

namespace hw
{

    void Led::initialize()
    {

    }

    void Led::update()
    {
        if(_direction)
        {
            _val = _val + 10;

            if(_val > 250)
            {
                _val = 250;
                _direction = false;
            }

            analogWrite(LED2_PIN, _val);
        }
        else
        {
            _val = _val - 10;

            if(_val < 0)
            {
                _val = 0;
                _direction = true;
            }

            analogWrite(LED2_PIN, _val);
        }

//        Serial.println(_val);
    }

    void Led::setNetworkLed(const int& value)
    {
        _networkLedValue = value;
        analogWrite(LED3_PIN, _networkLedValue);
    }
}
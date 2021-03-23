#pragma once

#include "Config.h"

namespace hw
{
   class Led
    {
    public:
        void initialize();

        static void setNetworkLed(const int& value);
        static void setBrightnessWhite(const int& value);
        static void setBrightnessBlue(const int& value);
        static void setBrightnessSwitchState(const int& value);
    };
}
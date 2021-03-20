#pragma once

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

    private:
        static const int LED1_PIN = 5;
        static const int LED2_PIN = 6;
        static const int LED3_PIN = 7;
        static const int LED4_PIN = 8;

        int _val = 0;
        bool _direction = true;
    };
}
#pragma once

namespace hw
{
   class Led
    {
    public:
        void initialize();
        void update();

        void setNetworkLed(const int& value);

    private:
        const int LED1_PIN = 5;
        const int LED2_PIN = 6;
        const int LED3_PIN = 7;

        int _val = 0;
        bool _direction = true;

        int _networkLedValue = 0;
    };
}
#pragma once

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Pzem004t.h"

namespace hw
{
    class DisplaySSD1306
    {
    public:
        DisplaySSD1306();
        virtual ~DisplaySSD1306() = default;

        void initialize();
        void update(const hw::pzem004tvalues& phasesCombined, const hw::pzem004tvalues& phase1Values, const hw::pzem004tvalues& phase2Values, const hw::pzem004tvalues& phase3Values);

        void tst()
        {
            col1x = 30;
        };

    private:
        Adafruit_SSD1306 _ssd1306;

        bool _isInitialized = false;


        int16_t col1x = 0;
        const int16_t col2x = 40;

        const int16_t col1y = 16;
        const int16_t colHeight = 23;
    };
}
#pragma once

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Pzem004t.h"

namespace hw
{
    enum class View
    {
        First,
        VoltagePowerFrequency,
        VoltageCurrentFrequency,
        CurrentPowerEnergy,
        VoltageFrequencyPowerFactor,
        Last
    };

    class DisplaySSD1306
    {
    public:
        DisplaySSD1306();
        virtual ~DisplaySSD1306() = default;

        void initialize();
        void update(const hw::pzem004tvalues& phasesCombined, const hw::pzem004tvalues& phase1Values, const hw::pzem004tvalues& phase2Values, const hw::pzem004tvalues& phase3Values);

        void drawBarGraph(const int16_t& x, const int16_t& y, const int16_t& width, const int16_t& height, const float& min, const float& max, const float& value);

        void switchView();
        void switchPhase();

        void showMessage(char* message);
        void clearMessage();

        void changeView(int value);

        const byte selectedPhase();
        const View selectedView();

    private:
        void drawVoltagePowerFrequency(const pzem004tvalues& values);
        void drawVoltageCurrentFrequency(const pzem004tvalues& values);
        void drawCurrentPowerEnergy(const pzem004tvalues& values);
        void drawVoltageFrequencyPowerFactor(const pzem004tvalues& values);
        void drawSelectedPhase();
        void drawMessage();

        const hw::pzem004tvalues& getSelectedValues(const hw::pzem004tvalues& phasesCombined, const hw::pzem004tvalues& phase1Values, const hw::pzem004tvalues& phase2Values, const hw::pzem004tvalues& phase3Values);

        Adafruit_SSD1306 _ssd1306;

        bool _isInitialized = false;

        char* _message = nullptr;

        float _maxCurrent = 1;

        int16_t col1x = 35;
        const int16_t col1y = 16;
        const int16_t colHeight = 23;

        byte _selectedPhase = 0; // 0 = all; 1-3 = phase1,2,3
        View _selectedView = View::VoltagePowerFrequency;
    };
}
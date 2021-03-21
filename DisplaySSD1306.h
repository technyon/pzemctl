#pragma once

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Pzem004t.h"

namespace hw
{
    enum class ViewValueType
    {
        Voltage = 0,
        Current = 1,
        Power = 2,
        Energy = 3,
        Frequency = 4,
        PowerFactor = 5
    };

    struct ViewConfiguration
    {
        ViewValueType value1;
        ViewValueType value2;
        ViewValueType value3;
    };

    class DisplaySSD1306
    {
    public:
        DisplaySSD1306();
        virtual ~DisplaySSD1306() = default;

        void initialize();
        void update(const hw::pzem004tvalues& phasesCombined, const hw::pzem004tvalues& phase1Values, const hw::pzem004tvalues& phase2Values, const hw::pzem004tvalues& phase3Values);

        void setCustomViewConfiguration(const ViewConfiguration& customViewConfiguration);

        void switchView();
        void switchPhase();

        void showMessage(char* message);
        void clearMessage();

        void changeView(int value);
        void changePhase(int value);

        const byte selectedPhase();
        const int selectedView();
        const ViewConfiguration& customViewConfiguration();

    private:
        void drawSelectedPhase();
        void drawMessage();
        void drawBarGraph(const int16_t& x, const int16_t& y, const int16_t& width, const int16_t& height, const float& min, const float& max, const float& value);
        void drawValueByType(int16_t x, int16_t y, const ViewValueType& type, const pzem004tvalues& values);

        const hw::pzem004tvalues& getSelectedValues(const hw::pzem004tvalues& phasesCombined, const hw::pzem004tvalues& phase1Values, const hw::pzem004tvalues& phase2Values, const hw::pzem004tvalues& phase3Values);

        #define NUM_VIEW_CONFIGURATIONS 4

        ViewConfiguration _viewConfigurations[NUM_VIEW_CONFIGURATIONS]
            {
                { ViewValueType::Voltage, ViewValueType::Power, ViewValueType::Frequency },
                { ViewValueType::Voltage, ViewValueType::Current, ViewValueType::Frequency },
                { ViewValueType::Current, ViewValueType::Power, ViewValueType::Energy },
                { ViewValueType::Voltage, ViewValueType::Frequency, ViewValueType::PowerFactor },
            };

        ViewConfiguration _customViewConfiguration;

        bool _isInitialized = false;
        char* _message = nullptr;
        float _maxCurrent = 1;
        int16_t col1x = 35;
        const int16_t col1y = 16;
        const int16_t colHeight = 23;

        byte _selectedPhase = 0; // 0 = all; 1-3 = phase1,2,3
        int _selectedView = 0;

        Adafruit_SSD1306 _ssd1306;
    };
}
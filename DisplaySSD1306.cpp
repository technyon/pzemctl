#include "DisplaySSD1306.h"
#include <Fonts/FreeSans12pt7b.h>

namespace hw
{

    DisplaySSD1306::DisplaySSD1306()
    : _ssd1306(128, 64, &Wire, -1)
    {

    }

    void DisplaySSD1306::initialize()
    {
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
        Serial.println(F("Inititalizig Display"));

        _isInitialized = _ssd1306.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
        if(!_isInitialized)
        {
            Serial.println(F("SSD1306 allocation failed"));
        }
        else
        {
            _ssd1306.setFont(&FreeSans12pt7b);
            _ssd1306.setTextColor(SSD1306_WHITE);

            _ssd1306.clearDisplay();

            _ssd1306.setCursor(10, 40);
            _ssd1306.print(F("Initializing"));
            _ssd1306.display();
        }
    }

    void DisplaySSD1306::update(const pzem004tvalues &phasesCombined, const pzem004tvalues &phase1Values, const pzem004tvalues &phase2Values, const pzem004tvalues &phase3Values)
    {
        if(!_isInitialized) return;

        _ssd1306.clearDisplay();

        _ssd1306.setCursor(col1x,col1y);
        _ssd1306.print(F("V:"));
        _ssd1306.setCursor(col2x,col1y);
        _ssd1306.print(phasesCombined.voltage, 1);

        _ssd1306.setCursor(col1x,col1y + 1 * colHeight);
        _ssd1306.print(F("A:"));
        _ssd1306.setCursor(col2x,col1y + 1 * colHeight);
        _ssd1306.print(phasesCombined.current, 1);

        _ssd1306.setCursor(col1x,col1y + 2 * colHeight);
        _ssd1306.print(F("Hz:"));
        _ssd1306.setCursor(col2x,col1y + 2 * colHeight);
        _ssd1306.print(phasesCombined.frequency, 1);

        _ssd1306.display();
    }
}
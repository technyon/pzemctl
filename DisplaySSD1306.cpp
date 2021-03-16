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
            _ssd1306.dim(false);

            _ssd1306.setFont(&FreeSans12pt7b);
            _ssd1306.setTextColor(SSD1306_WHITE);

            _ssd1306.clearDisplay();

            _ssd1306.setCursor(10, 40);
            _ssd1306.print(F("Initializing"));
            _ssd1306.display();
        }
    }

    void DisplaySSD1306::update(const pzem004tvalues& phasesCombined, const pzem004tvalues& phase1Values, const pzem004tvalues& phase2Values, const pzem004tvalues& phase3Values)
    {
        if(!_isInitialized) return;

        _ssd1306.clearDisplay();

        pzem004tvalues values = getSelectedValues(phasesCombined, phase1Values, phase2Values, phase3Values);

        drawSelectedPhase();
        drawBarGraph(5, 0, 25, 18, 10, 20, 19);

        _ssd1306.setCursor(col1x,col1y);
        _ssd1306.print(F("V:"));
        _ssd1306.setCursor(col2x,col1y);
        _ssd1306.print(values.voltage, 1);

        _ssd1306.setCursor(col1x,col1y + 1 * colHeight);
        _ssd1306.print(F("W:"));
        _ssd1306.setCursor(col2x,col1y + 1 * colHeight);
        _ssd1306.print(values.power, 1);

        _ssd1306.setCursor(col1x,col1y + 2 * colHeight);
        _ssd1306.print(F("Hz:"));
        _ssd1306.setCursor(col2x,col1y + 2 * colHeight);
        _ssd1306.print(values.frequency, 1);

        _ssd1306.display();
    }

    void DisplaySSD1306::drawBarGraph(int16_t x, int16_t y, int16_t width, int16_t height, float min, float max, float value)
    {
        int16_t fillWidth = ((value - min) / (max - min)) * (float)width;

        _ssd1306.drawRect(x, y, width, height, SSD1306_WHITE);
        _ssd1306.fillRect(x, y, fillWidth, height, SSD1306_WHITE);
    }

    void DisplaySSD1306::drawSelectedPhase()
    {
        switch(_selectedPhase)
        {
            case 0:
                _ssd1306.fillRect(0,0,3,18,SSD1306_WHITE);
                _ssd1306.fillRect(0,1 * colHeight,3,18,SSD1306_WHITE);
                _ssd1306.fillRect(0,2 * colHeight,3,18,SSD1306_WHITE);
                break;
            case 1:
                _ssd1306.fillRect(0,0,3,18,SSD1306_WHITE);
                break;
            case 2:
                _ssd1306.fillRect(0,1 * colHeight,3,18,SSD1306_WHITE);
                break;
            case 3:
                _ssd1306.fillRect(0,2 * colHeight,3,18,SSD1306_WHITE);
                break;
        }
    }

    void DisplaySSD1306::switchPhase()
    {
        _selectedPhase++;
        if(_selectedPhase > 3)
        {
            _selectedPhase = 0;
        }
    }

    const hw::pzem004tvalues& DisplaySSD1306::getSelectedValues(const pzem004tvalues& phasesCombined, const pzem004tvalues& phase1Values, const pzem004tvalues& phase2Values, const pzem004tvalues& phase3Values)
    {
        switch(_selectedPhase)
        {
            case 0:
                return phasesCombined;
            case 1:
                return phase1Values;
            case 2:
                return phase2Values;
            case 3:
                return phase3Values;
        }
        return phasesCombined;
    }
}
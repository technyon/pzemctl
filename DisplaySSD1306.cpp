#include "DisplaySSD1306.h"
#include <Fonts/FreeSans12pt7b.h>

namespace hw
{
    DisplaySSD1306::DisplaySSD1306()
    : _ssd1306(128, 64, &Wire, -1)
    { }

    void DisplaySSD1306::initialize()
    {
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
        Serial.println(F("Initializing Display"));

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

            _ssd1306.setCursor(15, 40);
            _ssd1306.print(F("Initializing"));
            _ssd1306.display();
        }
    }

    void DisplaySSD1306::update(const pzem004tvalues& phasesCombined, const pzem004tvalues& phase1Values, const pzem004tvalues& phase2Values, const pzem004tvalues& phase3Values)
    {
        if(!_isInitialized) return;
        long ts = millis();

        if(_clearMessageTs != -1 && ts >= _clearMessageTs)
        {
            _clearMessageTs = -1;
            _message = nullptr;
        }

        float lastMaxcurrent = _maxCurrent;
        _maxCurrent = max(_maxCurrent, max(max(phase1Values.current, phase2Values.current), phase3Values.current));
        if(_maxCurrent > lastMaxcurrent)
        {
            _expiresAt = ts + 10000;
        }

        if(ts > _expiresAt)
        {
            _maxCurrent = 1;
            _expiresAt = ts + 10000;
            return;
        }

        _ssd1306.clearDisplay();

        if(_message != nullptr)
        {
            drawMessage();
        }
        else
        {
            pzem004tvalues values = getSelectedValues(phasesCombined, phase1Values, phase2Values, phase3Values);

            drawSelectedPhase();
            drawBarGraph(5, 0, 25, 18, 0, _maxCurrent, phase1Values.current);
            drawBarGraph(5, colHeight * 1, 25, 18, 0, _maxCurrent, phase2Values.current);
            drawBarGraph(5, colHeight * 2, 25, 18, 0, _maxCurrent, phase3Values.current);

            ViewConfiguration viewConfiguration = _selectedView == NUM_VIEW_CONFIGURATIONS ? _customViewConfiguration : _viewConfigurations[_selectedView];

            drawValueByType(col1x,col1y,                    viewConfiguration.value1, values);
            drawValueByType(col1x,col1y + 1 * colHeight, viewConfiguration.value2, values);
            drawValueByType(col1x,col1y + 2 * colHeight, viewConfiguration.value3, values);
        }
        _ssd1306.display();
    }

    void DisplaySSD1306::drawBarGraph(const int16_t& x, const int16_t& y, const int16_t& width, const int16_t& height, const float& min, const float& max, const float& value)
    {
        int16_t fillWidth = ((constrain(value, min, max) - min) / (max - min)) * (float)width;

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

    void DisplaySSD1306::drawValueByType(int16_t x, int16_t y, const ViewValueType &type, const pzem004tvalues& values)
    {
        _ssd1306.setCursor(x, y);

        switch(type)
        {
            case ViewValueType::Voltage:
                _ssd1306.print(values.voltage, 1);
                _ssd1306.print(F(" V"));
                break;
            case ViewValueType::Current:
                _ssd1306.print((float)values.current, 1);
                _ssd1306.print(F(" A"));
                break;
            case ViewValueType::Power:
                _ssd1306.print((float)values.power, (int)(values.power < 1000 ? 1 : 0));
                _ssd1306.print(F(" W"));
                break;
            case ViewValueType::Energy:
            {
                float energy = values.energy + getEnergyOffset();
                _ssd1306.print(energy, energy < 1000 ? 1 : 0);
                _ssd1306.print(F(" kw"));
                break;
            }
            case ViewValueType::Frequency:
                _ssd1306.print(values.frequency, 1);
                _ssd1306.print(F(" Hz"));
                break;
            case ViewValueType::PowerFactor:
                _ssd1306.print(values.pf, 1);
                break;
        }
    }

    void DisplaySSD1306::switchView()
    {
        _selectedView++;

        if(_selectedView > NUM_VIEW_CONFIGURATIONS)
        {
            _selectedView = 0;
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

    float DisplaySSD1306::getEnergyOffset()
    {
        switch(_selectedPhase)
        {
            case 0:
                return _energyOffsetPhase1 + _energyOffsetPhase2 + _energyOffsetPhase3;
            case 1:
                return _energyOffsetPhase1;
            case 2:
                return _energyOffsetPhase2;
            case 3:
                return _energyOffsetPhase3;
        }    }


    void DisplaySSD1306::drawMessage()
    {
        _ssd1306.setCursor(0,col1y+3);
        _ssd1306.print(_message);
    }

    void DisplaySSD1306::showMessage(char *message, int duration)
    {
        _message = message;
        if(duration > 0)
        {
            _clearMessageTs = millis() + duration;
        }
    }

    void DisplaySSD1306::clearMessage()
    {
        _message = nullptr;
    }

    const byte DisplaySSD1306::selectedPhase()
    {
        return _selectedPhase;
    }

    const int DisplaySSD1306::selectedView()
    {
        return _selectedView;
    }

    void DisplaySSD1306::changeView(int value)
    {
        _selectedView = constrain(value+1, 0, NUM_VIEW_CONFIGURATIONS); // This is correct. View number 4 is custom view
    }

    void DisplaySSD1306::changePhase(int value)
    {
        _selectedPhase = constrain(value, 0, 3);
    }

    void DisplaySSD1306::setCustomViewConfiguration(const ViewConfiguration& customViewConfiguration)
    {
        _customViewConfiguration.value1 = (ViewValueType)constrain((int)customViewConfiguration.value1, 0, 5);
        _customViewConfiguration.value2 = (ViewValueType)constrain((int)customViewConfiguration.value2, 0, 5);
        _customViewConfiguration.value3 = (ViewValueType)constrain((int)customViewConfiguration.value3, 0, 5);
    }

    const ViewConfiguration &DisplaySSD1306::customViewConfiguration()
    {
        return _customViewConfiguration;
    }

    void DisplaySSD1306::setEnergyOffsetPhase1(const float &value)
    {
        _energyOffsetPhase1 = value;
    }
    void DisplaySSD1306::setEnergyOffsetPhase2(const float &value)
    {
        _energyOffsetPhase2 = value;
    }
    void DisplaySSD1306::setEnergyOffsetPhase3(const float &value)
    {
        _energyOffsetPhase3 = value;
    }
}
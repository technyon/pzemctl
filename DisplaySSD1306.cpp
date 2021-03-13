#include "DisplaySSD1306.h"
#include <Fonts/FreeMono12pt7b.h>

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

        if(!_ssd1306.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
            Serial.println(F("SSD1306 allocation failed"));
            for(;;); // Don't proceed, loop forever
        }

        _ssd1306.display();

        delay(2000);

        _ssd1306.clearDisplay();

        _ssd1306.setFont(&FreeMono12pt7b);
        _ssd1306.setTextColor(SSD1306_WHITE);

        _ssd1306.display();
    }

    void DisplaySSD1306::update(const pzem004tvalues &phasesCombined, const pzem004tvalues &phase1Values, const pzem004tvalues &phase2Values, const pzem004tvalues &phase3Values)
    {
        _ssd1306.clearDisplay();
        _ssd1306.setCursor(0,15);

        _ssd1306.print(F("V:  "));
        _ssd1306.println(phasesCombined.voltage, 1);

        _ssd1306.print(F("A:  "));
        _ssd1306.println(phasesCombined.current, 1);

        _ssd1306.print(F("Hz: "));
        _ssd1306.println(phasesCombined.frequency, 1);

        _ssd1306.display();
    }
}
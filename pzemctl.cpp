// Including Arduino.h is required for using Serial functions
#include <Arduino.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include "Pzem004t.h"
#include "Network.h"
#include <src/Arduino_FreeRTOS.h>
#include <src/task.h>
#include "DisplaySSD1306.h"
#include "Led.h"
#include "Input.h"
#include "WebServer.h"
#include "EepromConfiguration.h"
#include "Config.h"


EepromConfiguration* configuration;
hw::Pzem004t* pzem;
hw::Input* input;
hw::DisplaySSD1306* display;
Network *nw;
web::WebServer* webServer;
hw::Led led;

hw::pzem004tvalues phasesCombined;
hw::pzem004tvalues phase1Values;
hw::pzem004tvalues phase2Values;
hw::pzem004tvalues phase3Values;

bool setupModeRequested = false;
bool setupModeEnabled = false;

bool switchState = false;
uint8_t displayUpdateCounter = 0;

void setupMode()
{
    setupModeEnabled = true;
    Serial.println(F("SETUP"));
    display->showMessage("Starting\nSetup");
    nw->enableConfigMode();

    IPAddress ip = nw->ipAddress();
    char message[18];
    sprintf(message, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
    display->showMessage(message);

    webServer->enable();
    while(webServer->enabled() && setupModeEnabled)
    {
        vTaskDelay( 500 / portTICK_PERIOD_MS);
    }
    if(setupModeEnabled)
    {
        display->showMessage("Configu\nsaved");
    }
    else
    {
        display->showMessage("Setup\ncancelled");
    }

    webServer->disable();
    nw->disableConfigMode();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    display->clearMessage();

    setupModeEnabled = false;
}

void TaskPollPzem(void *pvParameters)
{
    while(true)
    {
        pzem->update();

        phase1Values = pzem->values1();
        phase2Values = pzem->values2();
        phase3Values = pzem->values3();

        phasesCombined.voltage = (phase1Values.voltage + phase2Values.voltage + phase3Values.voltage) / 3;
        phasesCombined.current = phase1Values.current + phase2Values.current + phase3Values.current;
        phasesCombined.power = phase1Values.power + phase2Values.power + phase3Values.power;
        phasesCombined.energy = phase1Values.energy + phase2Values.energy + phase3Values.energy;
        phasesCombined.frequency = (phase1Values.frequency + phase2Values.frequency + phase3Values.frequency) / 3;
        phasesCombined.pf = (phase1Values.pf + phase2Values.pf + phase3Values.pf) / 3;

        vTaskDelay( 100 / portTICK_PERIOD_MS);
    }
}

void TaskInputAndDisplay(void *pvParameters)
{
    while(true)
    {
        input->update();
        displayUpdateCounter++;
        if(displayUpdateCounter >= 4)
        {
            displayUpdateCounter = 0;
            display->update(phasesCombined, phase1Values, phase2Values, phase3Values);
        }
        vTaskDelay( 20 / portTICK_PERIOD_MS);
    }
}

void TaskNetwork(void *pvParameters)
{
    while(true)
    {
        nw->update(phasesCombined, phase1Values, phase2Values, phase3Values);

        vTaskDelay( 100 / portTICK_PERIOD_MS);
    }
}

void TaskSetupMode(void *pvParameters)
{
    while(true)
    {
        if(setupModeRequested)
        {
            setupModeRequested = false;
            if(!setupModeEnabled)
            {
                setupMode();
            }
        }

        vTaskDelay( 200 / portTICK_PERIOD_MS);
    }
}

void TaskWebServer(void *pvParameters)
{
    while(true)
    {
        webServer->update();
        vTaskDelay( 500 / portTICK_PERIOD_MS);
    }
}

void setupTasks()
{
    xTaskCreate(
            TaskPollPzem
            ,  "Pzm"
            ,  64
            ,  NULL
            ,  2
            ,  NULL );

    xTaskCreate(
            TaskInputAndDisplay
            ,  "Dsp"
            ,  256
            ,  NULL
            ,  0
            ,  NULL );

    xTaskCreate(
            TaskNetwork
            ,  "Nw"
            ,  976
            ,  NULL
            ,  1
            ,  NULL );

    xTaskCreate(
            TaskSetupMode
            ,  "Setup"
            ,  320
            ,  NULL
            ,  0
            ,  NULL );

    xTaskCreate(
            TaskWebServer
            ,  "WS"
            ,  448
            ,  NULL
            ,  0
            ,  NULL );
}

void setSwitchState(bool value, bool showMessage)
{
    if(switchState == value) return;

    switchState = value;
    digitalWrite(SWITCH_PIN, switchState);
    led.setBrightnessSwitchState(switchState ? 255 : 0);
    if(showMessage)
    {
        if (switchState)
        {
            display->showMessage("\n       ON", 1000);
        } else
        {
            display->showMessage("\n      OFF", 1000);
        }
    }
}

void buttonPressed(hw::ButtonId buttonId)
{
    switch(buttonId)
    {
        case hw::ButtonId::SwitchPhase:
            display->switchPhase();
            nw->publishPhase(display->selectedPhase());
            break;
        case hw::ButtonId::SwitchPhaseLong:
        {
            if(setupModeEnabled)
            {
                setupModeEnabled = false;
            }
            else
            {
                setupModeRequested = true;
            }

            break;
        }
        case hw::ButtonId::SwitchView:
            display->switchView();
            nw->publishView((int)display->selectedView() - 1);
            break;
        case hw::ButtonId::SwitchOnOff:
            setSwitchState(!switchState, true);
            nw->publishSwitchState(switchState);
            break;

    }
}

void configurationChanged()
{
    nw->configurationChanged();
}

void onNetworkEventReceived(const NetworkEvent& event)
{
    switch(event.type)
    {
        case NetworkEventType::viewChange:
            display->changeView(event.paramInt);
            break;
        case NetworkEventType::phaseChange:
            display->changePhase(event.paramInt);
            break;
        case NetworkEventType::switchStateChange:
            setSwitchState(event.paramBool, true);
            break;
        case NetworkEventType::resetEnergy:
            pzem->resetEnergy();
            display->showMessage("Energy\nreset");
            vTaskDelay( 1000 / portTICK_PERIOD_MS);
            display->clearMessage();
            break;
        case NetworkEventType::customViewValue1Changed:
        {
            hw::ViewConfiguration viewConfiguration = display->customViewConfiguration();
            viewConfiguration.value1 = (hw::ViewValueType) event.paramInt;
            display->setCustomViewConfiguration(viewConfiguration);
            break;
        }
        case NetworkEventType::customViewValue2Changed:
        {
            hw::ViewConfiguration viewConfiguration = display->customViewConfiguration();
            viewConfiguration.value2 = (hw::ViewValueType) event.paramInt;
            display->setCustomViewConfiguration(viewConfiguration);
            break;
        }
        case NetworkEventType::customViewValue3Changed:
        {
            hw::ViewConfiguration viewConfiguration = display->customViewConfiguration();
            viewConfiguration.value3 = (hw::ViewValueType) event.paramInt;
            display->setCustomViewConfiguration(viewConfiguration);
            break;
        }
        default:
            Serial.print(F("Unhandled network event type: "));
            Serial.print((int) event.type);
            Serial.println();
            break;
    }
}

void setup()
{
	Serial.begin(9600);
    Serial.println(F("Start"));

    randomSeed(analogRead(0) + analogRead(1) + analogRead(2));

    configuration = new EepromConfiguration(configurationChanged);

    input = new hw::Input(buttonPressed);
    nw = new Network(configuration, onNetworkEventReceived);
    webServer = new web::WebServer(nw->ethernetClient(), configuration);

    input->initialize();

    hw::ViewConfiguration viewConfiguration;
    viewConfiguration.value1 = hw::ViewValueType::Voltage;
    viewConfiguration.value2 = hw::ViewValueType::Current;
    viewConfiguration.value3 = hw::ViewValueType::PowerFactor;

    display = new hw::DisplaySSD1306();
    display->initialize();
    display->setCustomViewConfiguration(viewConfiguration);
    led.initialize();

    pinMode(SWITCH_PIN, OUTPUT);
    setSwitchState(true, false);

    pzem = new hw::Pzem004t(&Serial1, &Serial2, &Serial3);
    nw->initialize();

	setupTasks();

    setupModeRequested = !configuration->hasValidSignature();
}

void loop()
{

}

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
#include "Configuration.h"

Configuration* configuration;
hw::Pzem004t* pzem;
hw::Input* input;
hw::DisplaySSD1306 display;
Network *nw;
web::WebServer* webServer;
hw::Led led;

hw::pzem004tvalues phasesCombined;
hw::pzem004tvalues phase1Values;
hw::pzem004tvalues phase2Values;
hw::pzem004tvalues phase3Values;


void TaskBlink(void *pvParameters)
{
    while(true)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay( 250 / portTICK_PERIOD_MS);
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay( 250 / portTICK_PERIOD_MS);
    }
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
        phasesCombined.energy = phase1Values.energy + phase2Values.energy + phase3Values.energy;
        phasesCombined.frequency = (phase1Values.frequency + phase2Values.frequency + phase3Values.frequency) / 3;
        phasesCombined.pf = (phase1Values.pf + phase2Values.pf + phase3Values.pf) / 3;

        vTaskDelay( 200 / portTICK_PERIOD_MS);
    }
}

void TaskDisplay(void *pvParameters)
{
    while(true)
    {
        display.update(phasesCombined, phase1Values, phase2Values, phase3Values);

        vTaskDelay( 250 / portTICK_PERIOD_MS);
    }
}

void TaskLed(void *pvParameters)
{
    while(true)
    {
        led.update();

        vTaskDelay( 500 / portTICK_PERIOD_MS);
    }
}

void TaskInput(void *pvParameters)
{
    while(true)
    {
        input->update();

        vTaskDelay( 25 / portTICK_PERIOD_MS);
    }
}

void TaskNetwork(void *pvParameters)
{
    while(true)
    {
        nw->update(phase1Values, phase2Values, phase3Values);

        vTaskDelay( 100 / portTICK_PERIOD_MS);
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
            TaskBlink
            ,  "Blink"
            ,  128
            ,  NULL
            ,  0
            ,  NULL );

    xTaskCreate(
            TaskPollPzem
            ,  "PzemPoll"
            ,  128
            ,  NULL
            ,  2
            ,  NULL );

    xTaskCreate(
            TaskDisplay
            ,  "Display"
            ,  256
            ,  NULL
            ,  0
            ,  NULL );

    xTaskCreate(
            TaskLed
            ,  "Led"
            ,  128
            ,  NULL
            ,  0
            ,  NULL );

    xTaskCreate(
            TaskInput
            ,  "Input"
            ,  128
            ,  NULL
            ,  0
            ,  NULL );

    xTaskCreate(
            TaskNetwork
            ,  "Network"
            ,  1024
            ,  NULL
            ,  1
            ,  NULL );

    xTaskCreate(
            TaskWebServer
            ,  "WebServer"
            ,  512
            ,  NULL
            ,  0
            ,  NULL );

}

void buttonPressed(hw::ButtonId buttonId)
{
    switch(buttonId)
    {
        case hw::ButtonId::SwitchPhase:
            display.switchPhase();
            break;
        case hw::ButtonId::SwitchView:
            display.switchView();
            break;
    }
}

void configurationChanged()
{
    nw->configurationChanged();
}

void setup() {
	Serial.begin(9600);
    Serial.print("Start");

    configuration = new Configuration(configurationChanged);

//    Serial.print("# MQTT: ");
//    Serial.println(configuration->mqttServerAddress);

    input = new hw::Input(buttonPressed);
    nw = new Network(configuration);
    webServer = new web::WebServer(nw->ethernetClient(), configuration);

    input->initialize();
    display.initialize();
    led.initialize();

    pzem = new hw::Pzem004t(&Serial1, &Serial2, &Serial3);
    nw->initialize();
    webServer->initialize();

	pinMode(LED_BUILTIN, OUTPUT);

	setupTasks();
}

void loop()
{

}

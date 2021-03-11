// Including Arduino.h is required for using Serial functions
#include <Arduino.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include "pzem004t.h"
#include "network.h"
#include <src/Arduino_FreeRTOS.h>
#include <src/task.h>

hw::pzem004t* pzem;

network nw;

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
/*
        Serial.print("Voltage: ");
        Serial.print(phase1Values.voltage);
        Serial.print("V   |    ");
        Serial.print("Voltage: ");
        Serial.print(phase2Values.voltage);
        Serial.print("V   |    ");
        Serial.print("Voltage: ");
        Serial.print(phase3Values.voltage);
        Serial.println("V");
*/
        vTaskDelay( 200 / portTICK_PERIOD_MS);
    }
}

void TaskNetwork(void *pvParameters)
{
    while(true)
    {
        nw.update(phase1Values, phase2Values, phase3Values);

        vTaskDelay( 100 / portTICK_PERIOD_MS);
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
            TaskNetwork
            ,  "Network"
            ,  1024
            ,  NULL
            ,  1
            ,  NULL );

}

void setup() {
	Serial.begin(9600);
    Serial.print("Start");

    pzem = new hw::pzem004t(&Serial1, &Serial2, &Serial3);
    nw.initialize();

	pinMode(LED_BUILTIN, OUTPUT);

	setupTasks();
}

void loop()
{

}

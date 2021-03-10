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

//        Serial.print("Voltage: ");
//        Serial.print(phase1Values.voltage);
//        Serial.print("V   |    ");
//        Serial.print("Voltage: ");
//        Serial.print(phase2Values.voltage);
//        Serial.print("V   |    ");
//        Serial.print("Voltage: ");
//        Serial.print(phase3Values.voltage);
//        Serial.println("V");

        vTaskDelay( 200 / portTICK_PERIOD_MS);
    }
}

void TaskNetworkUpdate(void *pvParameters)
{
    while(true)
    {
        nw.update();

        vTaskDelay( 100 / portTICK_PERIOD_MS);
    }
}

void TaskNetworkPublish(void *pvParameters)
{
    while(true)
    {
        nw.publish(phase1Values, phase2Values, phase3Values);

        vTaskDelay( 1000 / portTICK_PERIOD_MS);
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
            TaskNetworkUpdate
            ,  "NetworkUpdate"
            ,  512
            ,  NULL
            ,  3
            ,  NULL );

    xTaskCreate(
            TaskNetworkPublish
            ,  "Publish"
            ,  1024
            ,  NULL
            ,  1
            ,  NULL );

}

// the setup routine runs once when you press reset:
void setup() {
	// initialize serial communication at 9600 bits per second:
	Serial.begin(9600);
    Serial.print("Start");

    pzem = new hw::pzem004t(&Serial1, &Serial2, &Serial3);
    nw.initialize();

	pinMode(LED_BUILTIN, OUTPUT);

	setupTasks();
}


// the loop routine runs over and over again forever:
void loop()
{
/*
    hw::pzem004tvalues pzemValues;

	// Blink the inbuilt LED

    pzem->update();

    pzemValues = pzem->values1();
    Serial.print("Voltage: ");
    Serial.print(pzemValues.voltage);
    Serial.print("V   |    ");
    pzemValues = pzem->values2();
    Serial.print("Voltage: ");
    Serial.print(pzemValues.voltage);
    Serial.print("V   |    ");
    pzemValues = pzem->values3();
    Serial.print("Voltage: ");
    Serial.print(pzemValues.voltage);
    Serial.println("V");

    nw.publish(pzem->values1(), pzem->values2(), pzem->values3());
    nw.update();

    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(250);                       // wait for a second
*/

/*
#ifdef LED_BUILTIN
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(250);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(250);                       // wait for a second
#endif
 */
}

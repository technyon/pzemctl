// Including Arduino.h is required for using Serial functions
#include <Arduino.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include "pzem004t.h"
#include "network.h"


hw::pzem004t* pzem;

network nw;

// the setup routine runs once when you press reset:
void setup() {
	// initialize serial communication at 9600 bits per second:
	Serial.begin(9600);
    Serial.print("Start");

    pzem = new hw::pzem004t(&Serial1, &Serial2, &Serial3);
    nw.initialize();

	// Setup to blink the inbuilt LED
#ifdef LED_BUILTIN
	pinMode(LED_BUILTIN, OUTPUT);
#endif

}

void loop()
{
    delay(1);
    nw.update();
}

// the loop routine runs over and over again forever:
void loopx()
{
    hw::pzem004tvalues pzemValues;

	// Blink the inbuilt LED
#ifdef LED_BUILTIN
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(250);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(250);                       // wait for a second

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

    nw.update();

    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(250);                       // wait for a second

#endif
}

// Including Arduino.h is required for using Serial functions
#include <PZEM004Tv30.h>
#include <Arduino.h>
#include <HardwareSerial.h>

PZEM004Tv30 pzem1(&Serial1);
PZEM004Tv30 pzem2(&Serial1);
PZEM004Tv30 pzem3(&Serial1);

// the setup routine runs once when you press reset:
void setup() {
	// initialize serial communication at 9600 bits per second:
	Serial.begin(9600);

	// print out hello world
	Serial.println("Hello World");

    pzem1.resetEnergy();
    pzem1.setAddress(0xf8);
    pzem2.resetEnergy();
    pzem2.setAddress(0xf8);
    pzem3.resetEnergy();
    pzem3.setAddress(0xf8);

	// Setup to blink the inbuilt LED
#ifdef LED_BUILTIN
	pinMode(LED_BUILTIN, OUTPUT);
#endif
}

// the loop routine runs over and over again forever:
void loop() {
	// Blink the inbuilt LED
#ifdef LED_BUILTIN
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(250);                       // wait for a second

    float volt = pzem1.voltage();
    Serial.print("Voltage: ");
    Serial.print(volt);
    Serial.print("V   |    ");

    volt = pzem2.voltage();
    Serial.print("Voltage: ");
    Serial.print(volt);
    Serial.print("V   |    ");

    volt = pzem3.voltage();
    Serial.print("Voltage: ");
    Serial.print(volt);
    Serial.println("V   |    ");



  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(250);                       // wait for a second
#endif
}

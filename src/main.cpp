#include <Arduino.h>

const int ledPin = 21;

const int dataPin = 33;
const int clockPin = 26;
const int loadPin = 25;


void setup()
{
	Serial.begin(9600);

	pinMode(dataPin, INPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(loadPin, OUTPUT);

	digitalWrite(clockPin, 0);
	digitalWrite(loadPin, 1);
}

void loop()
{
	// Load parallel inputs into shift register
	digitalWrite(loadPin, 0);
	digitalWrite(clockPin, 1);
	digitalWrite(clockPin, 0);

	// Shift inputs and read bit per bit into value
	digitalWrite(loadPin, 1);
	unsigned short value = 0;
	for (size_t bit = 0; bit < 16; bit++)
	{
		unsigned short read = digitalRead(dataPin);
		value = value | (read << bit);

		digitalWrite(clockPin, 1);
		digitalWrite(clockPin, 0);
	}

	Serial.println(value, BIN);
	delay(20);
}
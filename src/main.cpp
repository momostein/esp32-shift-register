#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "net_config.h"
#include "panel.h"

const int dataPin = 27;
const int clockPin = 26;
const int loadPin = 25;

static inline __attribute__((always_inline)) void directWriteLow(uint32_t pin)
{
	if (pin < 32)
		GPIO.out_w1tc = ((uint32_t)1 << pin);
	else if (pin < 34)
		GPIO.out1_w1tc.val = ((uint32_t)1 << (pin - 32));
}

static inline __attribute__((always_inline)) void directWriteHigh(uint32_t pin)
{
	if (pin < 32)
		GPIO.out_w1ts = ((uint32_t)1 << pin);
	else if (pin < 34)
		GPIO.out1_w1ts.val = ((uint32_t)1 << (pin - 32));
}

WiFiUDP udp;

void setup()
{

	pinMode(dataPin, INPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(loadPin, OUTPUT);

	directWriteLow(clockPin);
	directWriteHigh(loadPin);

	Serial.begin(9600);

	WiFi.begin(WLAN_SSID, WLAN_PWD);
	Serial.print("Connecting");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(WLAN_SSID);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	udp.begin(UDP_PORT);
}

void loop()
{
	// Load parallel inputs into shift register
	directWriteLow(loadPin);
	delayMicroseconds(1);
	directWriteHigh(clockPin);
	delayMicroseconds(1);
	directWriteLow(clockPin);
	delayMicroseconds(1);

	// Shift inputs and read bit per bit into value
	directWriteHigh(loadPin);

	Panel panels[1];

	for (size_t panelIndex = 0; panelIndex < sizeof(panels) / sizeof(Panel); panelIndex++)
	{
		for (size_t regIndex = 0; regIndex < 3; regIndex++)
		{
			uint8_t reg = 0;

			for (size_t bit_i = 0; bit_i < 8; bit_i++)
			{

				uint8_t read = (GPIO.in >> dataPin) & 1;
				Serial.print(read, BIN);

				reg |= (read << bit_i);

				delayMicroseconds(1);
				directWriteHigh(clockPin);
				delayMicroseconds(1);
				directWriteLow(clockPin);
				delayMicroseconds(1);
			}

			panels[panelIndex][regIndex] = reg;
		}
	}

	Serial.print(" - ");

	// Serial.println(value, BIN);

	for (size_t panelIndex = 0; panelIndex < sizeof(panels) / sizeof(Panel); panelIndex++)
	{
		for (size_t regIndex = 0; regIndex < 3; regIndex++)
		{
			uint8_t reg = panels[panelIndex][regIndex];
			Serial.printf("%02X", reg);
		}
		Serial.print(" ");
	}
	Serial.println();

	udp.beginPacket(UDP_ADDRESS, UDP_PORT);
	udp.write((uint8_t *)(&panels), sizeof(panels));
	udp.endPacket();

	delay(50);
}

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "net_config.h"
#include "panel.h"
#include "fast_io.h"

const size_t num_panels = 5;
struct Panels
{
	Panel panels[num_panels];
};

const int dataPin = 27;
const int clockPin = 26;
const int loadPin = 25;

WiFiUDP udp;

IPAddress target_address;
uint16_t target_port;

const int CHECK_INTERVAL = 4;

Panels read_panels()
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

	Panels panels;

	for (size_t panelIndex = 0; panelIndex < sizeof(panels) / sizeof(Panel); panelIndex++)
	{
		for (size_t regIndex = 0; regIndex < 3; regIndex++)
		{
			uint8_t reg = 0;

			for (size_t bit_i = 0; bit_i < 8; bit_i++)
			{

				uint8_t read = (GPIO.in >> dataPin) & 1;

				reg |= (read << bit_i);

				delayMicroseconds(1);
				directWriteHigh(clockPin);
				delayMicroseconds(1);
				directWriteLow(clockPin);
				delayMicroseconds(1);
			}

			panels.panels[panelIndex].registers[regIndex] = reg;
		}
	}

	return panels;
}

void print_message(unsigned char *data, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		Serial.print(data[i], HEX);
	}
	Serial.println();
}

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
	Serial.print(WiFi.localIP());
	Serial.print(":");
	Serial.println(UDP_PORT);

	udp.begin(UDP_PORT);
	udp.flush();

	while (udp.parsePacket() < 4)
	{
		delay(100);
	}

	unsigned char data[4];
	udp.read(data, 4);
	print_message(data, 4);

	target_address = udp.remoteIP();
	target_port = udp.remotePort();

	Serial.println(target_address);
	Serial.println(target_port);
	udp.flush();
}

void printPanels(const Panels &panels)
{
	for (size_t panelIndex = 0; panelIndex < sizeof(panels) / sizeof(Panel); panelIndex++)
	{
		for (size_t regIndex = 0; regIndex < 3; regIndex++)
		{
			uint8_t reg = panels.panels[panelIndex].registers[regIndex];
			Serial.printf("%02X", reg);
		}
		if (panelIndex < (sizeof(panels) / sizeof(Panel)) - 1)
			Serial.print(" ");
	}
	Serial.println();
}

void loop()
{
	if (udp.parsePacket() >= 4)
	{
		unsigned char data[4];
		udp.read(data, 4);
		print_message(data, 4);

		target_address = udp.remoteIP();
		target_port = udp.remotePort();
	}

	for (int check_loop = 0; check_loop < CHECK_INTERVAL; check_loop++)
	{
		Panels panels = read_panels();

		// printPanels(panels);

		udp.beginPacket(target_address, target_port);
		udp.write((uint8_t *)(&panels), sizeof(panels));
		udp.endPacket();

		delay(50);
	}
}

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "net_config.h"
#include "shift_reg.h"
#include "panel.h"
#include "fast_io.h"

// Set your Static IP address
const IPAddress local_IP(10, 0, 0, 180);
// Set your Gateway IP address
const IPAddress gateway(10, 0, 0, 1);

const IPAddress subnet(255, 255, 0, 0);
const IPAddress primaryDNS(8, 8, 8, 8);	  // optional
const IPAddress secondaryDNS(8, 8, 4, 4); // optional

const int UDP_PORT = 3333; // The port that this ESP is listening on

const int PANELS_PER_CHANNEL = 2;
const int REGISTERS_PER_PANEL = 3;
const int BITS_PER_REGISTER = 8;

const int CHANNEL_PINS[] = {
	13, // D13 -> 2 -> 1
	12, // D12 -> 4 -> 3
	14, // D14 -> 6 -> 5
	27, // D27 -> 8 -> 7
	26, // D26 -> 10 -> 9
	25, // D25 -> 12 -> 11
	33, // D33 -> 14 -> 13
	32, // D32 -> 16 -> 15
	35, // D35 -> 18 -> 17
	34, // D34 -> 20 -> 19
};

const int CHANNEL_COUNT = sizeof(CHANNEL_PINS) / sizeof(int);

WiFiUDP udp;

IPAddress target_address;
uint16_t target_port;

void setup()
{
	shiftRegInit();
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		pinMode(CHANNEL_PINS[i], INPUT);
	}

	Serial.begin(9600);

	// Configures static IP address
	if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
	{
		Serial.println("STA Failed to configure");
	}

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

	target_address = udp.remoteIP();
	target_port = udp.remotePort();

	Serial.print("Target set to ");
	Serial.print(target_address);
	Serial.print(":");
	Serial.println(target_port);

	Serial.println(target_address);
	Serial.println(target_port);
	udp.flush();
}

void loop()
{
	byte registers[CHANNEL_COUNT * PANELS_PER_CHANNEL][REGISTERS_PER_PANEL] = {};

	if (udp.parsePacket() >= 4)
	{
		unsigned char data[4];
		udp.read(data, 4);

		target_address = udp.remoteIP();
		target_port = udp.remotePort();

		Serial.print("Target changed to ");
		Serial.print(target_address);
		Serial.print(":");
		Serial.println(target_port);
	}

	shiftRegSetModeStore();
	shiftRegClockPulse();

	shiftRegSetModeShift();
	for (int registerGroup = PANELS_PER_CHANNEL - 1; registerGroup >= 0; registerGroup--)
	{
		int panelOffset = (PANELS_PER_CHANNEL - 1 - registerGroup);

		for (int registerIndex = 0; registerIndex < REGISTERS_PER_PANEL; registerIndex++)
		{
			for (int bitIndex = 0; bitIndex < BITS_PER_REGISTER; bitIndex++)
			{
				shiftRegClockPulse();
				for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++)
				{
					int panelId = panelOffset + channelIndex * PANELS_PER_CHANNEL;

					byte value = directRead(CHANNEL_PINS[channelIndex]);
					registers[panelId][registerIndex] |= value << bitIndex;
				}
			}
		}
	}

	udp.beginPacket(target_address, target_port);
	udp.write((uint8_t *)(&registers), sizeof(registers));
	udp.endPacket();

	delay(50);
}

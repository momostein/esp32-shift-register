#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "fast_io.h"

// INCREASE THIS VALUE IF MEASUREMENTS ARE UNSTABLE
const int CLOCK_DELAY_US = 5;

const int S0_PIN = 4;
const int S1_PIN = 5;
const int CLOCK_PIN = 16;

// Must be held LOW
const int OUTPUT_DISABLE_PIN = 17;

// const uint32_t s0_bit = 1 << s0_pin;
// const uint32_t s1_bit = 1 << s1_pin;
// const uint32_t clock_bit = 1 << s1_pin;

void shiftRegInit()
{
	pinMode(S0_PIN, OUTPUT);
	pinMode(S1_PIN, OUTPUT);
	pinMode(CLOCK_PIN, OUTPUT);

	pinMode(OUTPUT_DISABLE_PIN, OUTPUT);
	digitalWrite(OUTPUT_DISABLE_PIN, LOW);
}

void shiftRegClockPulse()
{
	delayMicroseconds(CLOCK_DELAY_US);
	directWriteHigh(CLOCK_PIN);

	delayMicroseconds(CLOCK_DELAY_US);
	directWriteLow(CLOCK_PIN);

	delayMicroseconds(CLOCK_DELAY_US);
}

inline void shiftRegSetModeStore()
{
	directWriteHigh(S0_PIN);
	directWriteHigh(S1_PIN);
}

inline void shiftRegSetModeShift()
{
	directWriteHigh(S0_PIN);
	directWriteLow(S1_PIN);
}

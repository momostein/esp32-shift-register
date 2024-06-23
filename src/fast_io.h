#pragma once	

#include <Arduino.h>

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
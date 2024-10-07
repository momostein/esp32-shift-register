#pragma once	

#include <Arduino.h>
#include <stdint.h>

#include "fast_io.h"

const int delay_us = 50;

const int s0_pin = 4;
const int s1_pin = 5;
const int clock_pin = 16;

const int st_output_disable = 17;

const uint32_t s0_bit = 1 << s0_pin;
const uint32_t s1_bit = 1 << s1_pin;
const uint32_t clock_bit = 1 << s1_pin;

void shift_reg_init() {
	pinMode(s0_pin, OUTPUT);
	pinMode(s1_pin, OUTPUT);
	pinMode(clock_pin, OUTPUT);

	pinMode(st_output_disable, OUTPUT);
	digitalWrite(st_output_disable, LOW);
}


void shift_reg_parallel_load() {
	const uint32_t store_bits_set =  s0_bit | s1_bit;

	delayMicroseconds(delay_us);
	// GPIO.out_w1tc = clock_bit;
	directWriteLow(clock_pin);

	delayMicroseconds(delay_us);
	// GPIO.out_w1ts = store_bits_set;
	directWriteHigh(s0_pin);
	directWriteHigh(s1_pin);

	delayMicroseconds(delay_us);
	directWriteHigh(clock_pin);
}

uint32_t shift_reg_shift_in()   {
	const uint32_t shift_bits_set =  s0_bit;
	const uint32_t shift_bits_clear =  s1_bit;

	delayMicroseconds(delay_us);
	directWriteLow(clock_pin);

	delayMicroseconds(delay_us);
	//  shift right: S0=L
	directWriteHigh(s0_pin);
	directWriteLow(s1_pin);

	delayMicroseconds(delay_us);
	directWriteHigh(clock_pin);

	delayMicroseconds(delay_us);
	return GPIO.in;
}
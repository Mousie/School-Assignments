/*
 * Lab08.c
 *
 * Created: 11/15/2020 1:51:11 AM
 * Author : Christopher Kihano
 * RedID: 824744811
 *
 * For this lab assignment, you must program the EEPROM unit of your AVR device
 * dynamically during run time. The functionality of your code should be as 
 * follows:
 * • You should download your code to the device only once!
 * • If the program runs for the first time, your code should set up a PWM 
 *   waveform with 0% duty cycle.
 * • Whenever your AVR decide is unplugged and plugged back in, the duty cycle 
 *   the waveform should be increased by 10%.
 * • After 100%, the duty cycle should cycle back to 0%.
 *
 * For example, the following is a sample sequence (Remember that the code is 
 * downloaded to the device only once. Every “next time” below means the device
 * is unplugged and plugged back in):
 * • If the AVR is running the code for the first time, the duty cycle % of the
 *   waveform is 0.
 * • If the AVR is running the code for the next time, the duty cycle % of the 
 *   waveform is 10.
 * • If the AVR is running the code for the next time, the duty cycle % of the 
 *   waveform is 20.
 * • …
 * • If the AVR is running the code for the next time, the duty cycle % of the
 *   waveform is 90.
 * • If the AVR is running the code for the next time, the duty cycle % of the
 *   waveform is 100.
 * • If the AVR is running the code for the next time, the duty cycle % of the 
 *   waveform is 0.
 * • …
 *
 * To demonstrate the duty cycle of the waveform, you should use a timer with 
 * 5ms period and demonstrate the PWM waveform on the on-board LED (remember 
 * Lab 4). Successful demonstration of your implementation includes finding the
 * parameters to setup this timer too.
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define EEPROMLOCATION 13

// Turn LED ON
ISR(TIMER0_COMPA_vect){
	if(OCR0B != 0) PORTB |= (1 << PORTB5);
}

// Turn LED OFF
ISR(TIMER0_COMPB_vect){
	PORTB &= ~(1 << PORTB5);
}

// Lookup table for our LED on time amounts
const uint8_t values[] PROGMEM = {
	0, 7, 15, 23, 30, 38, 46, 53, 61, 69, 76
};

int main(void) {
	DDRB = (1 << DDRB5);
	// LED Timer
	// Set TIMER0 to use CTC mode and pre-scalar of 1024
	// We're given a time of 0.005 seconds.
	// Plugging this into our equation for CTC timers,
	// (F_clk/N)*T-1 where F_clk = 16000000, N = prescaler of 1024, T = 0.005
	// The above equation gives me a value of 77.125, rounded down to 77.
	// Set to CTC Mode
	TCCR0A |= (1 << WGM01);
	// Pre-scalar of 1024
	TCCR0B |= (1 << CS02) | (1 << CS00);
	OCR0A = 77;
	// Enable compare match A and compare match B
	TIMSK0 |= (1 << OCIE0A) | (1 << OCIE0B);
	sei();
	
	// Get index offset stored in our EEPROM from last run.
	uint8_t index = eeprom_read_byte((uint8_t*) EEPROMLOCATION);
	// If the offset falls outside of the range we've defined or is the initial
	// value, "0xFF", reset to zero.
	if(index >= sizeof(values)) index = 0;
	// Use index on our lookup table to find the correct on time.
	OCR0B = pgm_read_byte(&(values[index]));
	// Write back the new index.
	eeprom_write_byte((uint8_t*) EEPROMLOCATION, ++index);
    /* Replace with your application code */
    while (1) {}
}


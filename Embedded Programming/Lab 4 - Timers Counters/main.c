/*
 * Lab04.c
 *
 * Created: 10/8/2020 9:40:40 AM
 * Author : Christopher Kihano
 *
 * Lab 3 Description:
 * For this lab assignment, you must use the Timers/Counters on the AVR board.
 * Here are the detailed functions, which will adjust the LED brightness based 
 * on the pushbutton:
 * • The brightness of the LED should increase from the current brightness when
 *   the button is pressed (ON) and decrease when the button is released (OFF)
 * • In the main() loop, when the pushbutton is pressed, generate a PWM 
 *   waveform to drive the LED so that the ON time (duty cycle) varies from 0 
 *   to 100% in about a second so it is clearly visible and stopping at 100% 
 *	 (fully ON) until the button is released
 * • When the pushbutton is released, the duty cycle should be reduced, from 
 *   the brightness at the point when the button is released until it is 
 *   completely OFF (0% duty cycle). If the button is released after achieving
 *   100% brightness, the LED should go from Full brightness (100% duty cycle) 
 *   to 0% duty cycle in about 1 second
 * • Use a timer to generate PWM waveform having a total time cycle 
 *   (Ton + Toff) of 10 millisecond.
 */ 

#include <avr/io.h>

int main(void){
	
	// LED Setup
	PORTB &= ~(1 << PORTB5);	// Set LED low
	DDRB |= (1 << DDRB5);		// Set LED to output
	
	// Timer Setup
	TCCR0A |= (1 << WGM01); // Set the Timer Mode to CTC
	OCR0A = 156; //represents 10ms timer
	OCR0B = 150; // Start at zero, meaning the LED is off. 
	TCCR0B |= (1 << CS02) | (1 << CS00); // set pre-scaler to 1024 and start the timer
	
	// Button Setup
	DDRB &= ~(1 << DDRB7);	// Ensure enable on PB7
	PORTB |= (1 << PORTB7);	// Pull up resistor on PB7
	
	while (1){
		// Prevents the "phantom light glow"
		if(OCR0B != 0) PORTB |= (1 << PORTB5);
		
		while ( (TIFR0 & (1 << OCF0B) ) == 0){} // wait for OCR0B overflow event
		TIFR0 |= (1 << OCF0B); // reset OCR0B overflow flag
		PORTB &= ~(1 << PORTB5);
		while ( (TIFR0 & (1 << OCF0A) ) == 0){} // wait for OCR0A overflow event
		TIFR0 |= (1 << OCF0A); // reset OCR0A overflow flag
		
		// Check button press
		if(!(PINB & (1 << PINB7))){
			if(OCR0B < 0x9C) {
				// This check happens ~10 ms. So we need to be able to go from 0 to 156 in about 100 loops.
				// 156/100 ~ 1.56 increase per loop. If we choose to increase by 1 on each loop, it would take ~1.5 
				//seconds but at 2 on each loop, it would take ~0.75 seconds. My solution is to oscillate between
				// adding 1 and adding 2.
				OCR0B += (OCR0B & 1) ? 1 : 2;    
			}
		}
		else{
			if(0 < OCR0B) OCR0B -= (OCR0B & 1) ? 1 : 2; 
		}
	}
}
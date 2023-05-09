/*
 * Lab01.c
 *
 * Created: 9/4/2020 2:48:10 PM
 * Author : Christopher Kihano
 * RedID: 824744811
 * CompE 375: Embedded Systems Programming Lab, Section 5 F 1-3:15PM
 */ 

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdbool.h>

#define TDOT 200
#define TDASH 600
#define TSPACE 200
#define TBETWEENLETTERS 400 // All dots/dash automatically have 200 ms. delay after them. Only need to delay remaining time based on type of spacing.
#define TBETWEENWORDS 800 // Using only 800 here since we already wait 600 ms after each letter regardless. 800+600=1400 
#define TAFTERFIRSTLETTER 200 // "... put a longer space between the first letter of each word."

bool addedDelay = false; // "... put a longer space between the first letter of each word."

// Letters and numbers are encoded using XXXDDDDD format.
// XXX refers to the number of dots or dashes for that letter or number.
// DDDDD encodes a 1 for a dash and 0 for a dot. Remaining bits are filled with zeros.
// Example: L has 4 dots/dashes so 100. It goes dot dash dot dot so 0100. Resultant binary is 10001000
uint8_t letters[] = {
	0b01001000, // A, DOT DASH
	0b10010000, // B, DASH DOT DOT DOT
	0b10010100, // C, DASH DOT DASH DOT
	0b01110000, // D, DASH DOT DOT
	0b00100000, // E, DOT
	0b01100100, // F, DOT DOT DASH DOT
	0b01111000, // G, DASH DASH DOT
	0b10000000, // H, DOT DOT DOT DOT
	0b01000000, // I, DOT DOT
	0b10001110, // J, DOT DASH DASH DASH
	0b01110100, // K, DASH DOT DASH
	0b10001000, // L, DOT DASH DOT DOT
	0b01011000, // M, DASH DASH
	0b01010000, // N, DASH DOT
	0b01111100, // O, DASH DASH DASH
	0b10001100, // P, DOT DASH DASH DOT
	0b10011010, // Q, DASH DASH DOT DASH
	0b01101000, // R, DOT DASH DOT
	0b01100000, // S, DOT DOT DOT
	0b00110000, // T, DASH
	0b01100100, // U, DOT DOT DASH
	0b10000010, // V, DOT DOT DOT DASH
	0b01101100, // W, DOT DASH DASH
	0b10010010, // X, DASH DOT DOT DASH
	0b10010110, // Y, DASH DOT DASH DASH
	0b10011000, // Z, DASH DASH DOT DOT
};

uint8_t numbers[] = {
	0b10111111, // 0, DASH DASH DASH DASH DASH
	0b10101111, // 1, DOT DASH DASH DASH DASH
	0b10100111, // 2, DOT DOT DASH DASH DASH
	0b10100011, // 3, DOT DOT DOT DASH DASH
	0b10100001, // 4, DOT DOT DOT DOT DASH
	0b10111111, // 5, DOT DOT DOT DOT DOT
	0b10110000, // 6, DASH DOT DOT DOT DOT
	0b10111000, // 7, DASH DASH DOT DOT DOT
	0b10111100, // 8, DASH DASH DASH DOT DOT
	0b10111110, // 9, DASH DASH DASH DASH DOT	
};

void write_bits(uint8_t bits){
	uint8_t actions = (bits & 0b11100000) >> 5;	// Mask out the top 3 bits, where we encode the length. Then shift it to get it to the right position.
	bits <<= 3;	// Removing the length bits and getting the data bits into the right position.
	while(actions--){
		PORTB |= 1 << PORTB5;
		if(bits & 0b10000000)
			_delay_ms(TDASH);
		else
			_delay_ms(TDOT);
		PORTB &= !(1 << PORTB5);
		_delay_ms(TSPACE);
		bits <<= 1;
	}
	if(addedDelay){ // "... put a longer space between the first letter of each word."
		_delay_ms(TAFTERFIRSTLETTER);
		addedDelay = false;
	}
}

void write_message(char* message){
	uint8_t character = *message;
	while(character != 0){	// Check for end of string.
		if(character == ' ') {
			PORTB &= !(1 << PORTB5);
			_delay_ms(TBETWEENWORDS);
			addedDelay = true; 
		}
		if(character >= 'a' && character <= 'z') // If lowercase letter, convert to upper.
			character &= 0b11011111; // Clear bit that determine uppercase-ness
		if(character >= 'A' && character <= 'Z')
			write_bits(letters[character - 'A']);
		else if(character >= '0' && character <= '9')
			write_bits(numbers[character - '0']);
		PORTB &= !(1 << PORTB5);
		_delay_ms(TBETWEENLETTERS);
		character = *(++message);	// Increment pointer, de-reference next character and load into character.
	}
	return;
}

int main(void){
	PORTB = 0; // Reset all outputs to low.
	DDRB |= 1<<DDRB5; // We'll be using B5 for output as that's the one connected to the on-board LED.
    
	char nameAndID[] = "Christopher Kihano Red ID 824744811";
	while(1)
		write_message(nameAndID);
		_delay_ms(2000);
}


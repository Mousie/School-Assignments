/*
 * Lab05.c
 *
 * Created: 10/14/2020 
 * Author : Christopher Kihano
 * RedID: 824744811
 *
 * This lab assignment intends to turn your keypad into a musical keyboard by
 * setting it up in such a way that, each key generates a different note of
 * frequency when pressed. Each button on the keypad should generate one of the
 * notes on the equally tempered scale where middle A is exactly 440Hz. Your 
 * code should output tones conforming to musical notes which are 
 * 2^(1/12) = ~1.059463 higher or lower than the adjacent note, starting at 
 * Middle A=440Hz. There are 12 notes: 5 sharp/flat notes and the 8 base notes.
 * The next higher note is A# (A-sharp) or 440* 2^(1/12) Hz = ~466.16376Hz and
 * B is 440* 2^(1/12) * 2^(1/12) = 493.883Hz, etc.
 *
 * Each note is related to the adjacent note by 12√2 ≈ 1.059463, such that 
 * after twelve note divisions (one octave) the frequency (e.g.: A below middle
 * A) is divided by a factor of 2 or 220Hz: (12√2) 12 = 2.
 *
 * To calculate that frequency, you use the equation 1/(440 * (2^(1/12))^x ) ) 
 * where ‘x’ is the key number (index) starting from 0 for key ‘1’ and working 
 * to 15 for key ‘D’. The program scans to find which key is pressed in the 
 * same way it did for lab 4, going one row at a time checking each column in
 * that row to see which key is pressed. The code should then interpret that 
 * button based on the pins each key is associated with, and output the 
 * corresponding sound frequency for each letter, number, and symbol. Based on
 * which key is pressed, a case statement sets the frequency of the noise. For
 * final demonstration, implement this using Timers.
 *
 * NOTE: Use the keypad scan function from lab 4, use the timers to generate 
 * musical tones on an output pin connected to headphones or speakers. You can
 * ignore button de-bouncing for this lab
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/pgmspace.h>
	
void timer2_init(){
	// Set to Fast PWM Mode and toggle OC2A on match. This will produce a 50% duty cycle
	TCCR2A |= (1 << COM2A0) |  (1 << WGM21) | (1 << WGM20);
	// Set to Fast PWM Mode & Set Pre-Scalar to 256
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << WGM22);
}

// This uses FREQ = F_CPU/(DIVIDER * (1+2*TOP)) but rewritten to solve for TOP rather than FREQ.
uint8_t const frequencies[4][4] PROGMEM = {
	{41, 49, 59, 70},	// * 7 4 1
	{39, 46, 55, 66},	// 0 8 5 2
	{37, 44, 52, 62},	// # 9 6 3
	{1, 1, 1, 1}		// D C B A Going to use letter keys to play songs?
};

void keypad_init(){
		// Set outputs
		DDRD |= (1 << DDRD4) | (1 << DDRD5) | (1 << DDRD6) | (1 << DDRD7);
		// Set all outputs as high
		PORTD |= (1 << PORTD4) | (1 << PORTD5) | (1 << PORTD6) | (1 << PORTD7);
		// Set inputs
		DDRD &= ~((1 << DDRD0) | (1 << DDRD1) | (1 << DDRD2) | (1 << DDRD3));
		// Enable pullup resistors
		PORTD |= (1 << PORTD0) | (1 << PORTD1) | (1 << PORTD2) | (1 << PORTD3);
}


int main(void)
{
	keypad_init();
	timer2_init();
	
	uint8_t inputs, offset = 0;
	for(;;){
		for(uint8_t i = 0; i < 4; i++){
			PORTD = (~(1 << 4+i) & 0xF0) | (PORTD & 0x0F);
			asm ("nop");
			asm ("nop");
			inputs = PIND & 0x0F;
			if(inputs != 0b00001111){
				// If things match up, figure out which button it was.
				if(inputs == 0x07) offset = 0;
				else if(inputs == 0x0B) offset = 1;
				else if(inputs == 0x0D) offset = 2;
				else if(inputs == 0x0E) offset = 3;
				else continue;
				DDRB |= (1 << DDRB3);
				OCR2A = pgm_read_byte(&(frequencies[i][offset]));
				// If the button is held down, don't print duplicate characters
				while((PIND & 0x0F) == inputs) {}
				DDRB &= ~(1 << DDRB3);
			}
		}
		
	}
}
/*
 * Lab03.c
 *
 * Created: 9/18/2020 4:38:37 PM
 * Author : Christopher Kihano
 * RedID: 824744811
 *
 * Lab 3 Description:
 * For this Lab assignment, your task is to scan the 4x4 keypad and transmit
 * the corresponding ASCII character using the USART. Your program is intended
 * to output the letter or the number that is pushed on the sixteen button 
 * keypad provided to you, based on which key is being pressed. The suggestion
 * is that you set rows as output and columns as input. The code shall first 
 * set all rows high at each pass and then, go one row at a time (setting them
 * low), checking each column in that row, to identify which button is pressed.
 * And then interpret the pressed button based on the pins each key is 
 * associated with, and output the key’s corresponding letter/number/symbol on
 * the Data Visualizer console provided in the Atmel Studio. The code should 
 * run on an infinite loop, continually scanning for any button that is pushed.
 *
 */ 

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>

#define BAUD 9600
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

char keypad[4][4] = {
		{'A', 'B', 'C', 'D'},
		{'3', '6', '9', '#'},
		{'2', '5', '8', '0'},
		{'1', '4', '7', '*'}
	};

void USART_init(){
	UBRR0H = BAUDRATE >> 8;
	UBRR0L = BAUDRATE;
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
}

void txByte(uint8_t byte){
	while(!(UCSR0A & (1 << UDRE0))){}
	UDR0 = byte;
}


int main(void)
{
	// Set outputs
	DDRD |= (1 << DDRD4) | (1 << DDRD5) | (1 << DDRD6) | (1 << DDRD7);
	// Set all outputs as high
	PORTD |= (1 << PORTD4) | (1 << PORTD5) | (1 << PORTD6) | (1 << PORTD7);
	// Set inputs
	DDRB &= ~((1 << DDRB0) | (1 << DDRB1) | (1 << DDRB2) | (1 << DDRB3));
	// Enable pullup resistors
	PORTB |= (1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3);
	
	USART_init();
	
	uint8_t inputs, offset = 0;
	for(;;){
		for(uint8_t i = 0; i < 4; i++){
			PORTD = (~(1 << 4+i) & 0xF0) | (PORTD & 0x0F);
			_delay_ms(1);	// Wait for port to be configured correctly.
			inputs = PINB & 0x0F;
			if(inputs != 0b00001111){
				// Wait for debounce to settle
				_delay_ms(50);
				// Double check that the button read is the button being pressed down.
				if(inputs != (PINB & 0x0F)) continue;
				// If things match up, figure out which button it was.
				if(inputs == 0x07) offset = 0;
				else if(inputs == 0x0B) offset = 1;
				else if(inputs == 0x0D) offset = 2;
				else if(inputs == 0x0E) offset = 3;
				else continue;
				txByte(keypad[i][offset]);
				// If the button is held down, don't print duplicate characters
				while((PINB & 0x0F) == inputs) {}
			}
		}
		
	}
}
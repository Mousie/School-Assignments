/*
 * Lab02.c
 *
 * Created: 9/12/2020 12:17:39 PM
 * Author : Christopher Kihano
 * RedID: 824744811
 * CompE 375: Embedded Systems Programming Lab, Section 5 F 1-3:15PM
 */

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#define BAUD 9600
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

const char redID[] PROGMEM = "824744811\n\r";
uint8_t idIndex = 0;

void USART_init(){
	UBRR0H = BAUDRATE >> 8;
	UBRR0L = BAUDRATE;
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
}

int main(void)
{
	USART_init();
	
	for(;;){
		while(!(UCSR0A & (1 << UDRE0))){}
		if(idIndex == 11){
			_delay_ms(500);
			idIndex = 0;
		}
		else UDR0 =  pgm_read_byte(redID+(idIndex++));
	}
}


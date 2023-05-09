/*
 * Lab07.c
 *
 * Created: 11/6/2020 2:27:00 AM
 * Author : Christopher Kihano
 * RedID: 824744811
 * 
 * For this lab assignment, you must use the A/D (analog to digital converter 
 * or ADC) to convert analog input signals in the range 0 to the reference 
 * voltage. In this case, we will use the power supply voltage as the reference.
 *
 * This program uses timers and interrupts to make an LED dim and brighten. It
 * also aims to teach us about analog to digital conversion. The program 
 * requires setting up of two timers:
 *  i. one to control the LED brightness,
 * ii. one to start the A/D conversion process.
 *
 * The task of this lab exercise is to control the brightness of your LED using
 * the potentiometer.
 * 
 * The brightness must be proportional to the position of the wiper in the 
 * potentiometer.
 * 1. Set up a timer to generate an interrupt every (X+1) ms -- the timer ISR 
 *    starts the A/D conversion and toggle one GPIO pin
 * 2. Set up the A/D ISR to read the A/D result, scale it as an unsigned 
 *    fixed-point number in the range 0 to 1, save it in a global short 
 *	  variable, and toggle a second GPIO pin. Note the A/D result is a two-byte 
 *    variable.
 * 3. Connect the potentiometer to A/D input channel Y%6 (Y modulo 6) to be 
 *    read by the ISR in #2 above (Follow image in presentation for connection
 *    instructions)
 * 4. In the main () loop, use the global A/D result variable and control one 
 *    of the timers to generate a PWM waveform that varies the brightness of 
 *    the LED on the board, based on the potentiometer position. The PWM 
 *    frequency should be (Z+1)*100 Hz.
 *
 * The X, Y and Z values above are based on the last three digits of your Red 
 * ID number as follows:
 * • Use the last 3 digits of your Red ID as in 812345XYZ
 * • (X+1) ms Interrupt period to start the A/D (sample rate)
 * • Potentiometer connected to A/D channel number Y%6
 * • PWM Frequency is (Z+1)*100 Hz
 *
 * For demonstration of your output, you must be able to show proper variation 
 * in the scale of your LED brightness at various positions of the wiper in the
 * potentiometer.
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

volatile float ADCValue, ADCValueOld;

ISR(ADC_vect){
	ADCValue = ADC / 1023.0;
	TIFR0 |= (1 << OCF0A);
}

// Turn LED ON
ISR(TIMER2_COMPA_vect){
	if(OCR2B != 0)
		PORTB |= (1 << PORTB5);
}

// Turn LED OFF
ISR(TIMER2_COMPB_vect){
	if(OCR2A != OCR2B)
		PORTB &= ~(1 << PORTB5);
}

void adc_init(){
	// Set DDRC0 to input
	DDRC &= ~(1 << DDRC0);
	// Set ADC input to AVCC. 
	// Set input channel to be on ADC(1%6) aka PC1.
	ADMUX |= (1 << REFS0) | (1 << MUX0);
	// Set ADC enable, ADC Auto Trigger Enable, ADC Interrupt Enable, pre-scalar of 128
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	// Set Timer/Counter0 compare match A as a trigger source.
	ADCSRB |= (1 << ADTS1) | (1 << ADTS0); 
}

void timer_init(){
	// ADC Timer
	// Set TIMER0 to use CTC mode and pre-scaler of 1024
	// RedID is 824744811 so the "X" digit would be an 8, 1ms + 8 ms = 9 ms
	// (F_clk/N)*T-1 where F_clk = 16000000, N = prescaler of 1024, T = 0.009
	// The above equation gives me a value of 139.625, truncated to 139.
	// CTC Mode
	TCCR0A |= (1 << WGM01);
	// Pre-scalar of 1024
	TCCR0B |= (1 << CS02) | (1 << CS00);
	OCR0A = 139;
	// Timer Interrupt Mask Register
	//TIMSK0 |= (1 << OCIE0A);
	
	// LED Timer
	// Set TIMER2 to use CTC mode and pre-scalar of 1024
	// RedID is 824744811 so the "Z" digit would be a 1, (1+1)*100 Hz=200 Hz
	// To find the amount of time for this frequency, it's 1/Hz or 1/200=0.005
	// Plugging this into our equation for CTC timers,
	// (F_clk/N)*T-1 where F_clk = 16000000, N = prescaler of 1024, T = 0.005
	// The above equation gives me a value of 77.125, rounded up to 78.
	// Set to CTC Mode
	TCCR2A |= (1 << WGM21);
	// Pre-scalar of 1024
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
	OCR2A = 78;
	// Enable compare match A and compare match B
	TIMSK2 |= (1 << OCIE2A) | (1 << OCIE2B);
}

int main(void)
{
	DDRB |= (1 << DDRB5);
	timer_init();
	adc_init();
	sei();
    /* Replace with your application code */
    while (1){
		if(ADCValue != ADCValueOld){
			ADCValueOld = ADCValue;
			OCR2B = ADCValue * 78.0;
		}
	}
}


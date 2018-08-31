/*
 * Lab Section: B21
 * Assignment: Lab #8 Exercise #1
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */

#include <avr/io.h>

void ADC_init()
{
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

int main(void)
{
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	ADC_init();
	
	unsigned short x;
	
	while (1)
	{
		x = ADC;
		PORTC = (char)(x & 0x00FF);
		PORTD = (char)(x >> 6);
	}
}

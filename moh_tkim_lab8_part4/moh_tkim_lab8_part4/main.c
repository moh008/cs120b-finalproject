/*
 * Lab Section: B21
 * Assignment: Lab #8 Exercise #4
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */

#include <avr/io.h>

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

int main(void)
{
    DDRC = 0xFF; PORTC = 0x00;
	unsigned const long MAX = 0x0061;// my home has value at 0x0061 with 10k ohm resistor
	ADC_init();
	
    while (1) 
    {
		if(ADC >= MAX)
		{
			PORTC = 0xFF;
		}
		else if(ADC >= 7*(MAX)/8)
		{
			PORTC = 0x7F;
		}
		else if(ADC >=  6*(MAX)/8)
		{
			PORTC = 0x3F;
		}
		else if(ADC >= 5*(MAX)/8)
		{
			PORTC = 0x1F;
		}
		else if(ADC >= 4*(MAX)/8)
		{
			PORTC = 0x0F;
		}
		else if(ADC >= 3*(MAX)/8)
		{
			PORTC = 0x07;
		}
		else if(ADC >= 2*(MAX)/8)
		{
			PORTC = 0x03;
		}
		else if((ADC < (MAX/4)) && (ADC >= 3*MAX/16))
		{
			PORTC = 0x01;
		}
		else if(ADC <= (3*MAX/16))
		{
			PORTC = 0x00;
		}
    }
}




/*
 * Lab Section: B21
 * Assignment: Lab #8 Exercise #3
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
	
	ADC_init();
	
    while (1) 
    {
		if(ADC >= (0x007F/2))
		{
			PORTC = 0x01;
		}
		else
		{
			PORTC = 0x00;
		}
    }
}




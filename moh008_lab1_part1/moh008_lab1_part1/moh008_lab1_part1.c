/*
 * Lab Section: B21
 * Assignment: Lab #1 Exercise #1
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */
#include <avr/io.h>

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	unsigned char tmpA = 0x00;
	unsigned char tmpB = 0x00;
	while(1)
	{
		tmpA = PINA & 0x0F;
		if(tmpA == 0x00)
		{
			tmpB = 0x00;
		}
		else if(tmpA == 0x01)
		{
			tmpB = 0x01;
		}
		else if(tmpA == 0x02)
		{
			tmpB = 0x00;
		}
		else if(tmpA == 0x03)
		{
			tmpB = 0x00;
		}
		PORTB = tmpB;
	}
	return 0;
}

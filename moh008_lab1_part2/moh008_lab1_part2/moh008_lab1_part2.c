/*
 * Lab Section: B21
 * Assignment: Lab #1 Exercise #2
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */
#include <avr/io.h>

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	
	unsigned char cntavail = 0x00;
	unsigned char tmpA = 0x00;
	
	while(1)
	{
		tmpA = PINA & 0x0F;
		if(tmpA == 0x0F)
		{
			cntavail = 0x00;
		}
		else if(tmpA == 0x01 || tmpA == 0x02 || tmpA == 0x04 || tmpA == 0x08)
		{
			cntavail = 0x03;
		}
		else if(tmpA == 0x0C || tmpA == 0x0A || tmpA == 0x09 || tmpA == 0x06 || tmpA == 0x05 || tmpA == 0x03)
		{
			cntavail = 0x02;
		}
		else if(tmpA == 0x0E || tmpA == 0x0D || tmpA == 0x0B || tmpA == 0x07)
		{
			cntavail = 0x01;
		}
		else if(tmpA == 0x00)
		{
			cntavail = 0x04;
		}
		PORTC = cntavail;
	}
	return 0;
}


/*
 * Partner Name & E-mail: Taehyun Kim[tkim058@ucr.edu]
 * Lab Section: B21
 * Assignment: Lab #2 Exercise #3
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */ 
#include <avr/io.h>

//bit-access function
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b)
{
	return (b? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k)
{
	return ((x & (0x01 << k)) != 0);
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	
	unsigned char tmpC = 0x00;
	unsigned char fuel = 0x00;
	
	while (1)
	{
		fuel = PINA & 0x70;
		if(GetBit(fuel, 4) && GetBit(fuel, 5) && !GetBit(fuel, 6))
		{
			tmpC = SetBit(tmpC, 7, 1);
		}
		else
		{
			tmpC = SetBit(tmpC, 7, 0);
		}
		
		fuel = PINA & 0x0F;
		
		if(fuel == 0x00)
		{
			tmpC = (tmpC & 0x80) | 0x40;
		}
		else if(fuel == 0x01 || fuel == 0x02)
		{
			tmpC = (tmpC & 0x80) | 0x60;
		}
		else if(fuel == 0x03 || fuel == 0x04)
		{
			tmpC = (tmpC & 0x80) | 0x70;
		}
		else if(fuel == 0x05 || fuel == 0x06)
		{
			tmpC = (tmpC & 0x80) | 0x38;
		}
		else if(fuel == 0x07 || fuel == 0x08 || fuel == 0x09)
		{
			tmpC = (tmpC & 0x80) | 0x3C;
		}
		else if(fuel == 0x0A || fuel == 0x0B || fuel == 0x0C)
		{
			tmpC = (tmpC & 0x80) | 0x3E;
		}
		else if(fuel == 0x0D || fuel == 0x0E || fuel == 0x0F)
		{
			tmpC = (tmpC & 0x80) | 0x3F;
		}
		PORTC = tmpC;
	}
}
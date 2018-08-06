/*
 * Partner Name & E-mail: Taehyun Kim[tkim058@ucr.edu]
 * Lab Section: B21
 * Assignment: Lab #4 Exercise #1
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */ 
#include <avr/io.h>

//same global function SetBit, GetBit as Part1

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b)
{
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k)
{
	return ((x & (0x01 << k)) != 0);
}
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	
	unsigned char led = 0x00;
	unsigned char button = 0x00;
	unsigned int fuel = 0;
	
	while(1)
	{
		button = ~PINA & 0xFF;						//declare variable fuel and read the value and convert to binary
		//value
		fuel = GetBit(button, 0) + 2*GetBit(button, 1) + 4*GetBit(button, 2) + 8*GetBit(button, 3);
		
		if((0 < fuel) && (fuel <= 2))		//set tmpC according to value of fuel
		{
			led = 0x60;
		}
		else if((3 <= fuel) && (fuel <= 4))
		{
			led = 0x70;
		}
		else if((5 <= fuel) && (fuel <= 6))
		{
			led = 0x38;
		}
		else if((7 <= fuel) && (fuel <= 9))
		{
			led = 0x3C;
		}
		else if((10 <= fuel) && (fuel <= 12))
		{
			led = 0x3E;
		}
		else if((13 <= fuel) && (fuel <= 15))
		{
			led = 0x3F;
		}
		else if(0 == fuel)
		{
			led = 0x40;
		}
		PORTC = led;						//save tmpC to PORTC
	}
}


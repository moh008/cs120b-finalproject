/*
 * Partner Name & E-mail: Taehyun Kim[tkim058@ucr.edu]
 * Lab Section: B21
 * Assignment: Lab #3 Exercise #1
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */ 

#include <avr/io.h>

enum States{B0_On, B1_On} state;
unsigned char tmpB = 0x00;
unsigned char A = 0x00;
// Bit-access function

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b)
{
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}

unsigned char GetBit(unsigned char x, unsigned char k)
{
	return ((x & 0x01) == 0);
}


void B_SM_Tick()
{
	switch (state)
	{
		case B0_On:
		if(GetBit(A, 0))
		{
			state = B0_On;
		}
		else
		{
			state = B1_On;
		}
		break;
		
		case B1_On:
		if(GetBit(A, 0))
		{
			state = B1_On;
		}
		else
		{
			state = B0_On;
		}
		break;
		
		default:
		break;
	}
	switch (state)
	{ //State Actions
		case B0_On:
		tmpB = SetBit(tmpB, 0, 1);
		tmpB = SetBit(tmpB, 1, 0);
		break;
		
		case B1_On:
		tmpB = SetBit(tmpB, 0, 0);
		tmpB = SetBit(tmpB, 1, 1);
		break;
		
		default:
		break;
	}
}

int main()
{
	state = B0_On;
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as input
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs


	while(1)
	{	A = PINA & 0x01;
		B_SM_Tick();
		// 3) write results to port
		PORTB = tmpB;
	}
	return 0;
}


/*
 * Partner Name & E-mail: Taehyun Kim[tkim058@ucr.edu]
 * Lab Section: B21
 * Assignment: Lab #3 Exercise #2
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */ 

#include <avr/io.h>
void Cntr_SM_Tick();
enum States{Init, Release, Inc, Dec, zero} state;
unsigned char tmpC = 0x00;
unsigned char A = 0x00;

void Cntr_SM_Tick()
{//state transition
	switch(state)
	{
		case Init:
		tmpC = 0x07;
		state = Release;
		break;
		
		case Release:
		if(A == 0x01)
		{
			state = Inc;
		}
		else if(A == 0x02)
		{
			state = Dec;
		}
		else if(A == 0x03)
		{
			state = zero;
		}
		else
		{
			state = Release;
		}
		break;
		
		case Inc:
		if(A == 0x03)
		{
			state = zero;
		}
		else if(A == 0x01)
		{
			state = Inc;
		}
		else
		{
			state = Release;
		}
		break;
		
		case Dec:
		if(A == 0x03)
		{
			state = zero;
		}
		else if(A == 0x02)
		{
			state = Dec;
		}
		else
		{
			state = Release;
		}
		break;
		
		case zero:
		state = Release;
		break;
		
		default:
		state = Init;
		break;
	}
	//state action
	switch(state)
	{
		case Init:
		break;
		
		case Release:
		break;
		
		case Inc:
		if(tmpC == 0x09)
		{
			break;
		}
		else
		{
			tmpC++;
		}
		break;
		
		case Dec:
		if(tmpC == 0x00)
		{
			break;
		}
		else
		{
			tmpC--;
		}
		break;
		
		case zero:
		tmpC = 0x00;
		break;
		
		default:
		break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	
	state = Init;
	
	while (1)
	{
		A = PINA & 0x03;
		Cntr_SM_Tick();
		PORTC = tmpC;
	}
	return 0;
}



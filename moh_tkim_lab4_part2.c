/*
 * Partner Name & E-mail: Taehyun Kim[tkim058@ucr.edu]
 * Lab Section: B21
 * Assignment: Lab #4 Exercise #2
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */ 
#include <avr/io.h>
void Cntr_SM_Tick();
enum States{Init, Release, Inc_pr, Inc_rl, Dec_pr, Dec_rl, zero_pr, zero_rl} state;
unsigned char led = 0x00;
unsigned char button = 0x00;

void Cntr_SM_Tick()
{//state transition
	switch(state)
	{
		case Init:
		led = 0x07;
		state = Release;
		break;
		
		case Release:
		if(button == 0x01)
		{
			state = Inc_pr;
		}
		else if(button == 0x02)
		{
			state = Dec_pr;
		}
		else if(button == 0x03)
		{
			state = zero_pr;
		}
		else
		{
			state = Release;
		}
		break;
		
		case Inc_pr:
		state = Inc_rl;
		break;
		
		case Inc_rl:
		if(button == 0x01)
		{
			state = Inc_rl;
		}
		else
		{
			state = Release;
		}
		break;
		
		case Dec_pr:
		state = Dec_rl;
		break;
		
		case Dec_rl:
		if(button == 0x02)
		{
			state = Dec_rl;
		}
		else
		{
			state = Release;
		}
		break;
		
		case zero_pr:
		state = zero_rl;
		break;
		
		case zero_rl:
		if((button == 0x03) ||(button == 0x01))
		{
			state = zero_rl;
		}
		else if(button == 0x00)
		{
			state = Release;
		}
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
		
		case Inc_pr:
		if(led == 0x09)
		{
			break;
		}
		else
		{
			led++;
		}
		break;
		
		case Inc_rl:
		break;
		
		case Dec_pr:
		if(led == 0x00)
		{
			break;
		}
		else
		{
			led--;
		}
		break;
		
		case Dec_rl:
		break;
		
		case zero_pr:
		led = 0x00;
		break;
		
		case zero_rl:
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
		button = ~PINA & 0x03;
		Cntr_SM_Tick();
		PORTC = led;
	}
	return 0;
}
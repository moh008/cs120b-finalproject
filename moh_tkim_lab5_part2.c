/*
 * Partner Name & E-mail: Taehyun Kim[tkim058@ucr.edu]
 * Lab Section: B21
 * Assignment: Lab #5 Exercise #2
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer.c"

enum States {Init, next_led, prev_led, stay_pr, released} state;

unsigned char tmpB = 0x00;
unsigned char button = 0x00;
unsigned char cnt = 0x00;

void Tick()
{
	switch(state)
	{//state transition
		case Init:
		if(button)
		{
			state = Init;
		}
		else
		{
			state = next_led;
			cnt = 0x00;
			tmpB = 0x01;
		}
		break;
		
		case next_led:
		if((tmpB == 0x04) && (cnt == 0x0A))
		{
			state = prev_led;
			tmpB = tmpB >> 1;
			cnt = 0;
		}
		else if(button)
		{
			state = stay_pr;
		}
		else
		{
			state = next_led;
		}
		break;
		
		case prev_led:
		if((tmpB == 0x01) && (cnt == 0x0A))
		{
			state = next_led;
			tmpB = tmpB << 1;
			cnt = 0;
		}
		else if(button)
		{
			state = stay_pr;
		}
		else
		{
			state = prev_led;
		}
		break;
		
		case stay_pr:
		if(!button)
		{
			state = released;
		}
		else if(button)
		{
			state = stay_pr;
		}
		else
		{
			break;
		}
		break;
		
		case released:
		if(button)
		{
			state = Init;
		}
		else if(!button)
		{
			state = released;
		}
		else
		{
			break;
		}
		break;
		
		default:
		break;
	}
	
	switch(state)
	{//state action
		case Init:
		break;
		
		case next_led:
		if(cnt == 0x0A)
		{
			tmpB = tmpB << 1;
			cnt = 0;
		}
		else
		{
			cnt++;
		}
		break;
		
		case prev_led:
		if(cnt == 0x0A)
		{
			tmpB = tmpB >> 1;
			cnt = 0;
		}
		else
		{
			cnt++;
		}
		break;
		
		case stay_pr:
		break;
		
		case released:
		break;
		
		default:
		break;
	}
}


void main()
{
	
	DDRA = 0x00;	DDRB = 0xFF;
	PORTA = 0xFF;	PORTB = 0x00;
	
	TimerSet(30);
	TimerOn();
	
	state = Init;

	while (1)
	{
		button = ~PINA;
		Tick();
		while(!TimerFlag){}
			TimerFlag = 0;
		PORTB = tmpB;
	}
}
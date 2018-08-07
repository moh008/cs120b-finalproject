/*
 * Partner Name & E-mail: Taehyun Kim[tkim058@ucr.edu]
 * Lab Section: B21
 * Assignment: Lab #5 Exercise #1
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer.c"

enum States {Init, B0_on, B1_on, B2_on} state;

unsigned char tmpB = 0x00;

void Tick()
{
	switch(state)
	{//state transition
		case Init:
		state = B0_on;
		break;
		
		case B0_on:
		state = B1_on;
		break;
		
		case B1_on:
		state = B2_on;
		break;
		
		case B2_on:
		state = B0_on;
		break;
		
		default:
		break;
	}
	
	switch(state)
	{//state action
		case Init:
		break;
		
		case B0_on:
		tmpB = 0x01;
		break;
		
		case B1_on:
		tmpB = 0x02;
		break;
		
		case B2_on:
		tmpB = 0x04;
		break;
		
		default:
		break;
		
	}
}


void main()
{
	DDRB = 0xFF;
	PORTB = 0x00;
	TimerSet(1000);	//Timer Period = 1000ms
	TimerOn();		//Turn Timer ON
    while (1) 
    {
		//User code (i.e. synchSM calls)
		Tick();
		PORTB = tmpB;
		while (!TimerFlag);	//wait 1 sec
		TimerFlag = 0;
		//Note: For the above a better style would use a synchSM with TickSM()
		//This example just illustrates the use of the ISR and flag
    }
}
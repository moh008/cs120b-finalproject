/*
 * moh_shiftreglabpart1.c
 *
 * Created: 2018-08-26 오후 10:26:32
 * Author : Lincoln
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "stdio.h"
#include "scheduler.h"
#include "Timer.c"

#define inc 0x01
#define dec 0x02

unsigned char val = 0x0F;

void transmit_data(unsigned char data)
{
	int i;
	for(i = 0; i < 8; ++i)
	{
		PORTB = 0x08;
		PORTB |= ((data >> i) & 0x01);
		PORTB |= 0x02;
	}
	PORTB |= 0x04;
	PORTB = 0x00;
}

void transmit_data2(unsigned char data)
{
	int i;
	for(i = 0; i < 8; ++i)
	{
		PORTC = 0x08;
		PORTC |= ((data >> i) & 0x01);
		PORTC |= 0x02;
	}
	PORTC |= 0x04;
	PORTC = 0x00;
}


enum SM1_States{sm1_init, sm1_wait, sm1_inc, sm1_dec};
	
int SM1_Tick(int state)
{
	unsigned char button = ~PINA & 0x03;
	switch(state)
	{
		case sm1_init:
		state = sm1_wait;
		break;
		
		case sm1_wait:
		if(button == inc)
		{
			state = sm1_inc;
		}
		else if(button == dec)
		{
			state = sm1_dec;
		}
		else
		{
			state = sm1_wait;
		}
		break;
		
		case sm1_inc:
		if(button == inc)
		{
			state = sm1_inc;
		}
		else
		{
			state = sm1_wait;
		}
		break;
		
		case sm1_dec:
		if(button == dec)
		{
			state = sm1_dec;
		}
		else
		{
			state = sm1_wait;
		}
		break;
		
		default:
		break;
	}
	switch(state)
	{
		case sm1_init:
		val = 0x00;
		break;
		
		case sm1_wait:
		break;
		
		case sm1_inc:
		if(val == 0xFF)
		{
			break;
		}
		else
		{
			val = (val << 1) | 0x01;
		}
		break;
		
		case sm1_dec:
		if(val == 0x00)
		{
			break;
		}
		
		else
		{
			val = (val >> 1) | 0x00;
		}
		break;
		
		default:
		break;
	}
	transmit_data(val);
	return state;
}
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	unsigned long int SM1_Tick_period = 10;
	
	static task task1;
	task *tasks[] = {&task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = 0;
	task1.period = SM1_Tick_period;
	task1.elapsedTime = SM1_Tick_period;
	task1.TickFct = &SM1_Tick;
	
	TimerSet(SM1_Tick_period);
	TimerOn();
	
	while (1) 
    {
		for(unsigned int i = 0; i < numTasks; i++)
		{
			if(tasks[i]->elapsedTime == tasks[i]->period)
			{
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}
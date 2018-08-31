/*
 * moh_matrixlab_sample.c
 *
 * Created: 2018-08-25 오후 5:48:33
 * Author : Lincoln
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Timer.c"
#include "scheduler.h"
#define up 0x01
#define down 0x02

//==========Global Variables===========
static unsigned char column_val = 0x01;//sets the pattern displayed on columns
static unsigned char column_sel = 0x00;//grounds column to display pattern

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
enum display_States{DISPLAY};
int display(int state){
	//===========Local Variables============
	unsigned char j = 0;
	//=============transitions==============
	switch(state){
		case DISPLAY:
		state = DISPLAY;
		break;
		
		default:
		state = DISPLAY;
		break;
	}
	//===============actions================
	switch(state){
		case DISPLAY:
		for(j = 0; j <= 7; j++)
		{
			transmit_data(column_sel);
			transmit_data2(column_val);
		}
		break;
		
		default:
		break;
	}
	return state;
}

enum SM1_States{sm1_init, sm1_wait, sm1_up, sm1_down};
int SM1_Tick(int state){
	//===========Local Variables============
	unsigned char button = ~PINA & 0x03;
	//=============transitions==============
	switch(state){
		case sm1_init:
		state = sm1_wait;
		break;
		
		case sm1_wait: 
		if(button == up)
		{
			state = sm1_up;
		}
		else if(button == down)
		{
			state = sm1_down;
		}
		else
		{
			state = sm1_wait;
		}
		break;
		
		case sm1_up:
		if(button == up)
		{
			state = sm1_up;
		}
		else
		{
			state = sm1_wait;
		}
		break;
		
		case sm1_down:
		if(button == down)
		{
			state = sm1_down;
		}
		else
		{
			state = sm1_wait;
		}
		break;
		
		default:
		state = sm1_init;
		break;
	}
	//===============actions================
	switch(state){
		case sm1_init:
		column_val = 0x01;
		column_sel = 0x00;
		break;
		
		case sm1_wait:	//if illuminated LED in bottom right corner
		break;
		
		case sm1_up:
		if(column_val == 0x80)
		{
			break;
		}
		else
		{
			column_val = (column_val << 1) | 0x00;
		}
		break;
		
		case sm1_down:
		if(column_val == 0x01)
		{
			break;
		}
		else
		{
			column_val = (column_val >> 1) | 0x00;
		}
		break;
		
		default:
		break;
	}
	
	return state;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	
	unsigned long int display_calc = 2;
	unsigned long int SM1_Tick_calc = 100;
	
	
	unsigned long GCD = findGCD(SM1_Tick_calc, display_calc);
	unsigned long int SM1_Tick1_period = SM1_Tick_calc/GCD;
	unsigned long int display_period = display_calc/GCD;
	
	static task task1, task2;
	task *tasks[] = {&task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = -1;
	task1.period = display_period;
	task1.elapsedTime = display_period;
	task1.TickFct = &display;
	
	
	task2.state = -1;
	task2.period = SM1_Tick1_period;
	task2.elapsedTime = SM1_Tick1_period;
	task2.TickFct = &SM1_Tick;
	
	TimerSet(GCD);
	TimerOn();
	
	while(1)
	{
		for(unsigned int i = 0; i < numTasks; i++)
		{
			if(tasks[i]->elapsedTime >= tasks[i]->period)
			{
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
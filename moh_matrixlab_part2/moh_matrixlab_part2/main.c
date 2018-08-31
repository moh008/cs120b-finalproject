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
#define left 0x04
#define right 0x08
//=======================
//SM1: DEMO LED matrix
//=======================
enum SM1_States{sm1_init, sm1_wait, sm1_left, sm1_right};
int SM1_Tick(int state){
	//===========Local Variables============
	static unsigned char column_val = 0xFF;//sets the pattern displayed on columns
	static unsigned char column_sel = 0xEF;//grounds column to display pattern
	unsigned char button = ~PINA & 0x0F;
	//=============transitions==============
	switch(state){
		case sm1_init:
		state = sm1_wait;
		break;
		
		case sm1_wait: 
		if(button == left)
		{
			state = sm1_left;
		}
		else if(button == right)
		{
			state = sm1_right;
		}
		else
		{
			state = sm1_wait;
		}
		break;
		
		case sm1_left:
		if(button == left)
		{
			state = sm1_left;
		}
		else
		{
			state = sm1_wait;
		}
		break;
		
		case sm1_right:
		if(button == right)
		{
			state = sm1_right;
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
		column_val = 0xFF;
		column_sel = 0x7F;
		break;
		
		case sm1_wait:	//if illuminated LED in bottom right corner
		break;
		
		case sm1_left:
		if(column_sel == 0x7F)
		{
			break;
		}
		else
		{
			column_sel = (column_sel << 1) | 0x01;
		}
		break;
		
		case sm1_right:
		if(column_sel == 0xFE)
		{
			break;
		}
		else
		{
			column_sel = (column_sel >> 1) | 0x80;
		}
		break;
		
		default:
		break;
	}
	PORTC = column_val;		//PORTC displays column pattern
	PORTB = column_sel;		//PORTB selects column to display pattern
	
	return state;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	
	unsigned long int SM1_Tick_calc = 100;
	unsigned long int tmpGCD = 1;
	unsigned long int GCD = tmpGCD;
	unsigned long int SM1_Tick1_period = SM1_Tick_calc;
	
	static task task1;
	task *tasks[] = {&task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = -1;
	task1.period = SM1_Tick1_period;
	task1.elapsedTime = SM1_Tick1_period;
	task1.TickFct = &SM1_Tick;
	
	TimerSet(GCD);
	TimerOn();
	
	while(1)
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
}
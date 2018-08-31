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
//=======================
//SM1: DEMO LED matrix
//=======================
enum SM1_States{sm1_display};
int SM1_Tick(int state){
	//==========Local Variables==========
	static unsigned char column_val = 0x01;//sets the pattern displayed on columns
	static unsigned char column_sel = 0x7F;//grounds column to display pattern
	
	//=========transitions========
	switch(state){
		case sm1_display: 
		break;
		
		default:
		state = sm1_display;
		break;
	}
	//=======actions============
	switch(state){
		case sm1_display:	//if illuminated LED in bottom right corner
		if(column_sel == 0xFE && column_val == 0x80)
		{
			column_sel = 0x7F;	//display far left column
			column_val = 0x01;	//pattern illuminates top row
		}
		// else if far right column was last to display (grounded)
		else if(column_sel == 0xFE)
		{
			column_sel = 0x7F;	//resets display column to far left column
			column_val = column_val << 1;	//shift down illuminated LED one row
		}
		//else shift displayed column one to the right
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
	DDRC = 0xFF; PORTC = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	
	unsigned long int SM1_Tick_calc = 100;
	unsigned long int tmpGCD = 1;
	unsigned long int GCD = tmpGCD;
	unsigned long int SM1_Tick1_period = SM1_Tick_calc;
	
	static task task1;
	task *tasks[] = {&task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = 0;
	task1.period = SM1_Tick1_period;
	task1.elapsedTime = SM1_Tick1_period;
	task1.TickFct = &SM1_Tick;
	
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
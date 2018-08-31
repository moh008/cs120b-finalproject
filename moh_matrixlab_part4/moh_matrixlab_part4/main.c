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

//==========shift register function===========
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
//=======================
//SM1: DEMO LED matrix
//=======================

//==============global variables==============
unsigned char column_val[8] = {0x00, 0x00, 0x3C, 0x24, 0x24, 0x3C, 0x00, 0x00};//sets the pattern displayed on columns
unsigned char column_sel[8] = {0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE};//grounds column to display pattern	
unsigned char side_location = 0;	//left side cursor location
unsigned int k = 0;
unsigned int j = 0;

enum SM1_States{sm1_display};
int SM1_Tick(int state){
	//===========Local Variables============
	//=============transitions==============
	switch(state){
		case sm1_display:
		state = sm1_display;
		break;
		
		default:
		state = sm1_display;
		break;
	}
	//===============actions================
	switch(state){
		case sm1_display:
		transmit_data(column_sel[j]);
		PORTC = column_val[j];
		break;
		
		default:
		break;
	}
	return state;
}

enum SM2_States{sm2_init, sm2_wait, sm2_moveup, sm2_movedown, sm2_moveleft, sm2_moveright};
int SM2_Tick(int state)
{
	//==============Local variables============
	unsigned char button = ~PINA & 0x0F;
	//=========================================
	switch(state)
	{//state transition
		case sm2_init:
		state = sm2_wait;
		break;
		
		case sm2_wait:
		if(button == up)
		{
			state = sm2_moveup;
		}
		else if(button == down)
		{
			state = sm2_movedown;
		}
		else if(button == left)
		{
			state = sm2_moveleft;
		}
		else if(button == right)
		{
			state = sm2_moveright;
		}
		else
		{
			state = sm2_wait;
		}
		break;
		
		case sm2_moveup:
		if(button == up)
		{
			state = sm2_moveup;
		}
		else
		{
			state = sm2_wait;
		}
		break;
		
		case sm2_movedown:
		if(button == down)
		{
			state = sm2_movedown;
		}
		else
		{
			state = sm2_wait;
		}
		break;
		
		case sm2_moveleft:
		if(button == left)
		{
			state = sm2_moveleft;
		}
		else
		{
			state = sm2_wait;
		}
		break;
		
		case sm2_moveright:
		if(button == right)
		{
			state = sm2_moveright;
		}
		else
		{
			state = sm2_wait;
		}
		break;
		
		default:
		state = sm2_init;
		break;
	}
	switch(state)
	{//state actions
		case sm2_init:
		k = 0;
		side_location = 2;
		break;
		
		case sm2_wait:
		break;
		
		case sm2_moveup:
		for(k = 0; k <= 7; k++)
		{
			if((column_val[k] == 0x90) || (column_val[k] == 0xF0))
			break;
			else
			column_val[k] = (column_val[k] << 1) | 0x00;
		}
		break;
		
		case sm2_movedown:
		for(k = 0; k <= 7; k++)
		{
			if((column_val[k] == 0x09) || (column_val[k] == 0x0F))
			break;
			else
			column_val[k] = (column_val[k] >> 1) | 0x00;
		}
		break;
		
		case sm2_moveleft:
		if(side_location <= 0)
		break;
		else
		{
			for(k = 0; k <= 7; k++)
			{
				column_sel[k] = (column_sel[k] << 1) | 0x01;
			}
		}
		side_location--;
		break;
		
		case sm2_moveright:
		if(side_location >= 4)
		break;
		else
		{
			for(k = 0; k <= 7; k++)
			{
				column_sel[k] = (column_sel[k] >> 1) | 0x80;
			}
		}
		side_location++;
		break;
		
		default:
		break;
	}return state;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	
	unsigned long int SM1_Tick_calc = 2;
	unsigned long int SM2_Tick_calc = 100;
	
	unsigned long int GCD = findGCD(SM1_Tick_calc, SM2_Tick_calc);
	unsigned long int SM1_Tick1_period = SM1_Tick_calc/GCD;
	unsigned long int SM2_Tick2_period = SM2_Tick_calc/GCD;
	
	static task task1, task2;
	task *tasks[] = {&task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = -1;
	task1.period = SM1_Tick1_period;
	task1.elapsedTime = SM1_Tick1_period;
	task1.TickFct = &SM1_Tick;
	
	task2.state = -1;
	task2.period = SM2_Tick2_period;
	task2.elapsedTime = SM2_Tick2_period;
	task2.TickFct = &SM2_Tick;
	
	TimerSet(GCD);
	TimerOn();
	
	while(1)
	{
		if(j == 7)
		j = 0;
		else
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
			j++;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}
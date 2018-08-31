/*
 * moh_matrixlab_part3.c
 *
 * Created: 2018-08-25 오후 5:48:33
 * Author : Lincoln
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Timer.c"
#include "scheduler.h"
#include "shiftreg.h"
#define p1_up 0x01
#define p1_down 0x02
#define p2_up 0x04
#define p2_down 0x08


//==============global variables==============
unsigned char p1_val[6] = {0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0};		//sets the pattern displayed on columns
unsigned char p1_sel = 0x7F;										//grounds column to display pattern
unsigned char p1_index = 0;												//p1 location index

unsigned char p2_val[6] = {0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0};
unsigned char p2_sel = 0xFE;
unsigned char p2_index = 0;												// p2 location index

unsigned char single = 0;		// player1 vs bot
unsigned char multi = 0;		// player1 vs player2
unsigned char ready = 0;		// when game is ready to start
unsigned char write = 0;		// write on LCD screen
unsigned char finish = 0;		// when game finished

unsigned char p1_win = 0;		//p1 win, score flag
unsigned char p2_win = 0;		//p2 win, score flag
unsigned char p1_score = 0;		//p1 score cumulative, stored in EEPROM
unsigned char p2_score = 0;		//p2 score cumulative

enum display_States{DISPLAY};
int display(int state){
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
		//for(j = 0; j <= 29; j++)
		//{
			transmit_data(p1_sel, p1_val[p1_index]);
			transmit_data(p2_sel, p2_val[p2_index]);
		//}
		/*for(j = 0; j <= 29; j++)
		{
			transmit_data(p2_sel);
			transmit_data2(p2_val[p2_index]);
		}
		*/
		break;
		
		default:
		break;
	}
	return state;
}

//P1_move
enum p1_move_States{p1_init, p1_wait, p1_moveup, p1_movedown};
int p1_move(int state)
{
	//==============Local variables============
	unsigned char button = ~PINA & 0x03;
	//=========================================
	switch(state)
	{//state transition
		case p1_init:
		state = p1_wait;
		break;
		
		case p1_wait:
		if(button == p1_up)
		{
			state = p1_moveup;
		}
		else if(button == p1_down)
		{
			state = p1_movedown;
		}
		else
		{
			state = p1_wait;
		}
		break;
		
		case p1_moveup:
		if(button == p1_up)
		{
			state = p1_moveup;
		}
		else
		{
			state = p1_wait;
		}
		break;
		
		case p1_movedown:
		if(button == p1_down)
		{
			state = p1_movedown;
		}
		
		else
		{
			state = p1_wait;
		}
		break;
		
		default:
		state = p1_init;
		break;
	}
	switch(state)
	{//state actions
		case p1_init:
		p1_sel = 0x7F;
		p1_index = 3;
		break;
		
		case p1_wait:
		break;
		
		case p1_moveup:
		if(p1_val[p1_index] == 0xE0)
		{
			break;
		}
		else
		{
			p1_index++;
		}
		break;
		
		case p1_movedown:
		if(p1_val[p1_index] == 0x07)
		{
			break;
		}
		else
		{
			p1_index--;
		}
		break;
		
		default:
		break;
	}
	return state;
}

//P2_move
enum p2_move_States{p2_init, p2_wait, p2_moveup, p2_movedown};
int p2_move(int state)
{
	//==============Local variables============
	unsigned char button = ~PINA & 0x0C;
	//=========================================
	switch(state)
	{//state transition
		case p2_init:
		state = p2_wait;
		break;
		
		case p2_wait:
		if(button == p2_up)
		{
			state = p2_moveup;
		}
		else if(button == p2_down)
		{
			state = p2_movedown;
		}
		else
		{
			state = p2_wait;
		}
		break;
		
		case p2_moveup:
		if(button == p2_up)
		{
			state = p2_moveup;
		}
		else
		{
			state = p2_wait;
		}
		break;
		
		case p2_movedown:
		if(button == p2_down)
		{
			state = p2_movedown;
		}
		else
		{
			state = p2_wait;
		}
		break;
		
		default:
		state = p2_init;
		break;
	}
	switch(state)
	{//state actions
		case p2_init:
		p2_sel = 0xFE;
		p2_index = 3;
		break;
		
		case p2_wait:
		break;
		
		case p2_moveup:
		if(p2_val[p2_index] == 0xE0)
		{
			break;
		}
		else
		{
			p2_index++;
		}
		break;
		
		case p2_movedown:
		if(p2_val[p2_index] == 0x07)
		{
			break;
		}
		else
		{
			p2_index--;
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
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	
	unsigned long int display_calc = 2;
	unsigned long int p1_move_calc = 50;
	unsigned long int p2_move_calc = 50;
	
	unsigned long int GCD = findGCD(display_calc, p1_move_calc);
	GCD = findGCD(GCD, p2_move_calc);
	
	unsigned long int display_period = display_calc/GCD;
	unsigned long int p1_move_period = p1_move_calc/GCD;
	unsigned long int p2_move_period = p2_move_calc/GCD;
	
	static task task1, task2, task3;
	task *tasks[] = {&task1, &task2, &task3};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = -1;
	task1.period = display_period;
	task1.elapsedTime = display_period;
	task1.TickFct = &display;
	
	task2.state = -1;
	task2.period = p1_move_period;
	task2.elapsedTime = p1_move_period;
	task2.TickFct = &p1_move;
	
	task3.state = -1;
	task3.period = p2_move_period;
	task3.elapsedTime = p2_move_period;
	task3.TickFct = &p2_move;
	
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
	return 0;
}
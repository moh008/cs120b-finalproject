/*
 * Lab Section: B21
 * Assignment: Lab #10 Exercise #2
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "bit.h"
#include "io.c"
#include "scheduler.h"
#include "Timer.c"

unsigned char myArray[68] = {' ',' ',' ',' ' ,' ',' ',' ',' ',' ',' ',' ',' ', ' ', ' ',' ','C','S','1','2','0','B',' ','i','s',' ','L','E','G','E','N','D','.','.','.',' ','w','a','i','t',' ','f','o','r',' ','i','t',' ','D','A','R','Y','!',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};

enum arrayState{disp} state;
unsigned char cursorCounter = 1;

int SMTick1(int state)
{
	switch(state)
	{
		case disp:
		for(int j = 1; j <= 16; j++)
		{
			LCD_Cursor(j);
			LCD_WriteData(myArray[cursorCounter+j-2]);
			if(cursorCounter+j+1 == 68)
			{
				cursorCounter = 1;
			}
		}
		cursorCounter++;
	}
	return state;
}

int main(void)
{
	DDRD = 0xFF; PORTD = 0x00;
	DDRA = 0xFF; PORTA = 0x00;
	
	unsigned long int SMTick1_calc = 300;
	
	unsigned long int tmpGCD = 1;
	
	unsigned long int GCD = tmpGCD;
	
	unsigned long int SMTick1_period = SMTick1_calc;
	
	static task task1;
	task *tasks[] = {&task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = 0;
	task1.period = SMTick1_period;
	task1.elapsedTime = SMTick1_period;
	task1.TickFct = &SMTick1;
	
	TimerSet(GCD);
	TimerOn();
	LCD_init();
	LCD_ClearScreen();
	
	unsigned char i;
	
	while(1) 
	{
		for(i = 0; i < numTasks; i++)
		{
			if(tasks[i] ->elapsedTime >= tasks[i]->period)
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
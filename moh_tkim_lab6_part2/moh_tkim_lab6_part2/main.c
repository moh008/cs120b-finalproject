/*
 * Lab Section: B21
 * Assignment: Lab #6 Exercise #2
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer.c"
#include "io.c"

enum States {Init, next_led, prev_led, stay_pr, released} state;

unsigned char tmpB = 0x00;
unsigned char button = 0x00;
unsigned char cnt = 0x00;
unsigned char score = 0x05;

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
			if(score == 0x09)
			{
				LCD_ClearScreen();
				score = 0x05;
				LCD_Cursor(1);
				LCD_WriteData(score + '0');
			}
			else if(score == 0x00)
			{
				LCD_ClearScreen();
				score = 0x05;
				LCD_Cursor(1);
				LCD_WriteData(score + '0');
			}
			
			else
			{
				LCD_Cursor(1);
				LCD_WriteData(score + '0');
			}
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
			if((tmpB == 0x02) && (score < 0x09))
			{
				score++;
				LCD_Cursor(1);
				LCD_WriteData(score + '0');
			}
			else if((tmpB != 0x02) && (score > 0x00))
			{
				score--;
				LCD_Cursor(1);
				LCD_WriteData(score + '0');
			}
			else
			{
				LCD_Cursor(1);
				LCD_WriteData(score + '0');
				break;
			}
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
			if((tmpB == 0x02) && (score < 0x09))
			{
				score++;
				LCD_Cursor(1);
				LCD_WriteData(score + '0');
			}
			else if((tmpB != 0x02) && (score > 0x00))
			{
				score--;
				LCD_Cursor(1);
				LCD_WriteData(score + '0');
			}
			else
			{
				LCD_Cursor(1);
				LCD_WriteData(score + '0');
				break;
			}
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
		else
		{
			state = stay_pr;
		}
		break;
		
		case released:
		if(button)
		{
			state = Init;
		}
		else
		{
			state = released;
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
		if(score == 0x09)
		{
			LCD_DisplayString(1, "Victory");
		}
		else if(score == 0x00)
		{
			LCD_DisplayString(1, "WAKE UP, HELLO???             ");
		}
		break;
		
		case released:
		break;
		
		default:
		break;
	}
}


void main()
{
	
	DDRA = 0x00;	PORTA = 0xFF;
	DDRB = 0xFF;	PORTB = 0x00;
	DDRC = 0xFF;	PORTC = 0x00;
	DDRD = 0xFF;	PORTD = 0x00;
	
	TimerSet(30);
	TimerOn();
	
	state = Init;
	LCD_init();
	
	while (1)
	{
		button = ~PINA;
		Tick();
		while(!TimerFlag){}
		TimerFlag = 0;
		PORTB = tmpB;
	}
}
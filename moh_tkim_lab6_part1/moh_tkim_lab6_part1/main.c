/*
 * Lab Section: B21
 * Assignment: Lab #6 Exercise #1
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
#include "Timer.c"

enum States{Init, Release, Inc, Dec, zero} state;

unsigned char led = 0x00;
unsigned char button = 0x00;

void Cntr_SM_Tick()
{//state transition
	switch(state)
	{
		case Init:
		led = 0x07;
		state = Release;
		break;
		
		case Release:
		if(button == 0x01)
		{
			state = Inc;
		}
		else if(button == 0x02)
		{
			state = Dec;
		}
		else if(button == 0x03)
		{
			state = zero;
		}
		else
		{
			state = Release;
		}
		break;
		
		case Inc:
		if(button == 0x01)
		{
			state = Inc;
		}
		else if(button == 0x02)
		{
			state = Dec;
		}
		else if(button == 0x03)
		{
			state = zero;
		}
		else
		{
			state = Release;
		}
		break;
		
		case Dec:
		if(button == 0x02)
		{
			state = Dec;
		}
		else if(button == 0x01)
		{
			state = Inc;
		}
		else if(button == 0x03)
		{
			state = zero;
		}
		else
		{
			state = Release;
		}
		break;
		
		case zero:
		if((button == 0x03) ||(button == 0x01))
		{
			state = zero;
		}
		else if(button == 0x01)
		{
			state = Inc;
		}
		else
		{
			state = Release;
		}
		break;
		
		default:
		state = Init;
		break;
	}
	
	switch(state)
	{//state actions
		case Init:
		break;
		
		case Release:
		break;
		
		case Inc:
		if(led == 0x09)
		{
			break;
		}
		else
		{
			led++;
		}
		break;
		
		case Dec:
		if(led == 0x00)
		{
			break;
		}
		else
		{
			led--;
		}
		break;
		
		case zero:
		led = 0x00;
		break;
		
		default:
		break;
	}
}

int main ( void )
{
	DDRA = 0x00; PORTA = 0xFF; // Input button
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	TimerSet(1000);
	TimerOn();
	
	state = Init;
	
	// Initializes the LCD display
	LCD_init ();
	while (1)
	{
		button = ~PINA & 0x03;
		Cntr_SM_Tick();
		LCD_Cursor(1);
		LCD_WriteData(led + '0');
		while(!TimerFlag){}
		TimerFlag = 0;
	}
}


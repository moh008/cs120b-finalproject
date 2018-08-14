/*
 * Lab Section: B21
 * Assignment: Lab #7 Exercise #3
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer.c"

unsigned char threeLEDs = 0x00;
enum TL_States{TL_init, TL_T0, TL_T1, TL_T2} TL_state;
void TickFct_ThreeLeds()
{
	switch(TL_state)
	{
		case TL_init:
		TL_state = TL_T0;
		break;
		
		case TL_T0:
		TL_state = TL_T1;
		break;
		
		case TL_T1:
		TL_state = TL_T2;
		break;
		
		case TL_T2:
		TL_state = TL_T0;
		break;
		
		default:
		break;
	}
	switch(TL_state)
	{
		case TL_init:
		threeLEDs = 0x00;
		break;
		
		case TL_T0:
		threeLEDs = (threeLEDs & 0xF8) | 0x01;
		break;
		
		case TL_T1:
		threeLEDs = (threeLEDs & 0xF8) | 0x02;
		break;
		
		case TL_T2:
		threeLEDs = (threeLEDs & 0xF8) | 0x04;
		break;
		
		default:
		break;
	}
}

unsigned char blinkingLED = 0x00;
enum BL_States {BL_init, BL_Off, BL_On} BL_state;
void TickFct_BlinkLed()
{
	switch(BL_state)
	{
		case BL_init:
		BL_state = BL_Off;
		break;
		
		case BL_Off:
		BL_state = BL_On;
		break;
		
		case BL_On:
		BL_state = BL_Off;
		break;
		
		default:
		break;
	}
	
	switch(BL_state)
	{
		case BL_init:
		blinkingLED = 0x00;
		break;
		
		case BL_Off:
		blinkingLED = 0x00;
		break;
		
		case BL_On:
		blinkingLED = (blinkingLED & 0xF0) | 0x08;
		break;
		
		default:
		break;
	}
}

unsigned char button = 0x00;
unsigned char buzz = 0x00;
unsigned char cnt = 0x00;
enum s_States {s_init, s_Off, s_On} s_state;
void TickFct_Sound()
{
	switch(s_state)
	{
		case s_init:
		s_state = s_Off;
		cnt = 0;
		break;
		
		case s_Off:
		if(button)
		{
			s_state = s_On;
		}
		else
		{
			s_state = s_Off;
		}
		break;
		
		case s_On:
		s_state = s_Off;
		break;
		
		default:
		break;
	}
	switch(s_state)
	{
		case s_init:
		buzz = (buzz & 0xEF);
		break;
		
		case s_Off:
		buzz = (buzz & 0xEF);
		break;
		
		case s_On:
		buzz = (buzz & 0xEF) | 0x10;
		break;
		
		default:
		break;
	}
}

enum CombineLED{C_init} C_state;
void TickFct_Combine()
{
	switch(C_state)
	{
		case C_init:
		PORTB = threeLEDs | blinkingLED | buzz;
		
		default:
		break;
	}
}
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	unsigned long TL_elapsedTime = 300;
	unsigned long BL_elapsedTime = 1000;
	unsigned long B_elapsedTime = 2;
	unsigned long B_cnt = 0;
	const unsigned long timer_Period = 1;
	
	
	TimerSet(timer_Period);
	TimerOn();
	
	TL_state = TL_init;
	BL_state = BL_init;
	s_state = s_init;
	
	while(1)
	{
		button = ~PINA & 0x04;
		if(button && B_elapsedTime >= 2 && B_cnt < 1000)
		{
			TickFct_Sound();
			B_elapsedTime = 0;
			B_cnt++;
		}
		else if(button && B_elapsedTime >= 2 && (B_cnt >= 1000) && (B_cnt < 3000))
		{
			B_cnt++;
		}
		else if(button && B_elapsedTime >= 2 && B_cnt >= 3000)
		{
			B_cnt = 0;
		}
		
		if(TL_elapsedTime >= 300)
		{
			TickFct_ThreeLeds();
			TL_elapsedTime = 0;
		}
		if(BL_elapsedTime >= 1000)
		{
			TickFct_BlinkLed();
			BL_elapsedTime = 0;
		}
		TickFct_Combine();
		while(!TimerFlag){}
		TimerFlag = 0;
		TL_elapsedTime += timer_Period;
		BL_elapsedTime += timer_Period;
		B_elapsedTime += timer_Period;
	}
}
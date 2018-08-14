/*
 * Lab Section: B21
 * Assignment: Lab #7 Exercise #5
 * I acknowledge all content contained herein, excluding template
	or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer.c"

enum pr_rl_States{p_init, p_released, p_Inc_pr, p_Inc_rl, p_Dec_pr, 
	p_Dec_rl, p_zero_pr, p_zero_rl} p_state;
unsigned char led = 0x00;
unsigned char button = 0x00;
	
void Prrl_Cntr_SM_Tick()
{
	switch(p_state)
	{
		case p_init:
		led = 0x07;
		p_state = p_released;
		break;
		
		case p_released:
		if(button == 0x01)
		{
			p_state = p_Inc_pr;
		}
		else if(button == 0x02)
		{
			p_state = p_Dec_pr;
		}
		else if(button == 0x03)
		{
			p_state = p_zero_pr;
		}
		else
		{
			p_state = p_released;
		}
		break;
		
		case p_Inc_pr:
		if(button == 0x01)
		{
			p_state = p_Inc_pr;
		}
		else
		{
			p_state = p_Inc_rl;
		}
		break;
		
		case p_Inc_rl:
		p_state = p_released;
		break;
		
		case p_Dec_pr:
		if(button == 0x02)
		{
			p_state = p_Dec_pr;
		}
		else
		{
			p_state = p_Dec_rl;
		}
		break;
		
		case p_Dec_rl:
		p_state = p_released;
		break;
		
		case p_zero_pr:
		if(button == 0x03)
		{
			p_state = p_zero_pr;
		}
		else
		{
			p_state = p_zero_rl;
		}
		break;
		
		case p_zero_rl:
		p_state = p_released;
		break;
		
		default:
		break;
	}
	switch(p_state)
	{
		case p_init:
		break;
		
		case p_released:
		break;
		
		case p_Inc_pr:
		break;
		
		case p_Inc_rl:
		if(led == 0x09)
		{
			break;
		}
		else
		{
			led++;
		}
		break;
		
		case p_Dec_pr:
		break;
		
		case p_Dec_rl:
		if(led == 0x00)
		{
			break;
		}
		else
		{
			led--;
		}
		break;
		
		case p_zero_pr:
		break;
		
		case p_zero_rl:
		led = 0x00;
		break;
		
		default:
		break;
	}
}

enum States{Init, Release, Inc, Dec, zero} state;
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
		else if(button == 0x00)
		{
			state = Release;
		}
		break;
		
		default:
		state = Init;
		break;
	}
	//state action
	switch(state)
	{
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

unsigned long flex_time = 1000;
unsigned char i = 0;
enum Inc_speed{I_wait, I_boost} I_state;
void TickFct_inc_speed()
{
	switch(I_state)
	{
		case I_wait:
		if(button == 0x01)
		{
			I_state = I_boost;
		}
		else
		{
			I_state = I_wait;
		}
		break;
		
		case I_boost:
		if(button == 0x01)
		{
			I_state = I_boost;
		}
		else
		{
			I_state = I_wait;
		}
		break;
		
		default:
		break;
	}
	switch(I_state)
	{
		case I_wait:
		flex_time = 1000;
		i = 0;
		break;
		
		case I_boost:
		flex_time = 400;
		break;
		
		default:
		break;
	}
}

enum Dec_speed{D_wait, D_boost} D_state;
void TickFct_Dec_speed()
{
	switch(D_state)
	{
		case D_wait:
		if(button == 0x02)
		{
			D_state = D_boost;
		}
		else
		{
			D_state = D_wait;
		}
		break;
		
		case D_boost:
		if(button == 0x02)
		{
			D_state = D_boost;
		}
		else
		{
			D_state = D_wait;
		}
		break;
		
		default:
		break;
	}
	switch(D_state)
	{
		case D_wait:
		flex_time = 1000;
		break;
		
		case D_boost:
		flex_time = 400;
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
		PORTB = led;
		
		default:
		break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	unsigned long Cntr_elapsedTime = 0;
	const unsigned long timer_Period = 10;
	
	TimerSet(timer_Period);
	TimerOn();
	
	state = Init;
	p_state = p_init;
	C_state = C_init;
	D_state = D_wait;
	I_state = I_wait;
	i = 0;
	
	while (1)
	{
		button = ~PINA & 0x03;
		Prrl_Cntr_SM_Tick();
		switch(button)
		{
			TickFct_inc_speed();
			case 0x01:
			if(Cntr_elapsedTime >= 1000)
			{
				if(i >= 3000)
				{
					TickFct_inc_speed();
				}
				else
				{
					Cntr_SM_Tick();
					Cntr_elapsedTime = 0;
					i += 1000;
				}
			}
			break;
			
			case 0x02:
			if((Cntr_elapsedTime >= flex_time) && i < 300)
			{
				Cntr_SM_Tick();
				Cntr_elapsedTime = 0;
				i++;
			}
			else if((Cntr_elapsedTime >= flex_time) && i >= 300)
			{
				TickFct_Dec_speed();
				Cntr_SM_Tick();
				Cntr_elapsedTime = 0;
			}
			break;
			
			default:
			break;
		}
		TickFct_Combine();
		while(!TimerFlag){}
		TimerFlag = 0;
		Cntr_elapsedTime += timer_Period;
	}
	return 0;
}


/* Assignment: Lab #9 Exercise #1
 * I acknowledge all content contained herein, excluding template
 or example code, is my own original work.
 * NOTE!!!!: This code targets PB6 NOT PB3
 */

#include <avr/io.h>
#include <avr/interrupt.h>

void set_PWM(double frequency) {
	static double current_frequency = 0x00;
	if (frequency != current_frequency) 
	{
		if (!frequency) TCCR3B &= 0x08; //stops timer/counter
		else TCCR3B |= 0x03; // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) OCR3A = 0xFFFF;
		
		// prevents OCR3A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) OCR3A = 0x0000;
		
		// set OCR3A based on desired frequency
		else OCR3A = (short)(8000000 / (128 * frequency)) - 1;

		TCNT3 = 0; // resets counter
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB6 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

unsigned char button = 0x00;
enum s_states {s_init, C, D, E} s_state;
void TickFct_Sound(){
	switch(s_state){
		case s_init:
		if(button == 0x01)
		{
			s_state = C;
		}
		else if(button == 0x02)
		{
			s_state = D;
		}
		else if(button == 0x04)
		{
			s_state = E;
		}
		else
		{
			s_state = s_init;
		}
		break;
		
		case C:
		if(button == 0x01)
		{
			s_state = C;
		}
		else
		{
			s_state = s_init;
		}
		break;
		
		case D:
		if(button == 0x02)
		{
			s_state = D;
		}
		else
		{
			s_state = s_init;
		}
		break;
		
		case E:
		if(button == 0x04)
		{
			s_state = E;
		}
		else
		{
			s_state = s_init;
		}
		break;
		
		default:
		s_state = s_init;
		break;
	}
	switch(s_state)
	{
		case s_init:
		set_PWM(0);
		PORTB = 0x00;
		break;
		
		case C:
		set_PWM(261.63);
		PORTB = 0x01;
		break;
		
		case D:
		set_PWM(293.66);
		PORTB = 0x02;
		break;
		
		case E:
		set_PWM(329.63);
		PORTB = 0x03;
		break;
		
		default:
		set_PWM(0);
		PORTB = 0x00;
		break;
	}
}
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	PWM_on();
	s_state = s_init;
	
	while(1)
	{
		button = ~PINA & 0x07;
		TickFct_Sound();
	}
	PWM_off();
}
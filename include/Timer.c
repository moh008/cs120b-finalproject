#include "Timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned long _avr_timer_M = 1;//Start count from here, down to 0. Default 1ms
unsigned long _avr_timer_cntcurr = 0;//Current internal count of 1ms ticks

void TimerOn()
{
	//avr timer/counter controller register TCCR1
	TCCR1B = 0x0B;	//bit3 = 0: CTC mode(clear timer on compare)
	//bit2bit1bit0 = 011: pre-scaler/64
	//0001011: 0x0B
	//so, 8MHz clock or 8,000,000/64 = 125,000 ticks/s
	//Thus, TCNT1 register will count at 125,000 ticks/s
	//AVR output compare register OCR1A.
	OCR1A = 125;	//Timer interrupt will be generated when TCNT1 == OCR1A
	//We want a 1ms tick. 0.001s * 125,000 ticks/s = 125
	//So when TCNT1 register equals 125,
	//1 ms has passed. Thus, we compare to 125.
	//AVR timer interrupt mask register
	TIMSK1 = 0x02;	// bit1: OCIE1A -- enables compare match interrupt
	
	//Initialize avr counter
	TCNT1 = 0;
	
	_avr_timer_cntcurr = _avr_timer_M;
	//TimerISR will be called every _avr_timer_cntcurr milliseconds
	
	//Enable global interrupts
	SREG |= 0x80;	//0x80: 1000000
}

void TimerOff()
{
	TCCR1B = 0x00;	// bit3bit1bit0 = 000: timer off
}

void TimerISR()
{
	TimerFlag = 1;
}

//In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect)
{
	//CPU automatically calls when TCNT1 == OCR1 (every 1ms per TimerOn settings)
	_avr_timer_cntcurr--;	//count down to 0 rather than up to TOP
	if(_avr_timer_cntcurr == 0)
	{
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

//Set TimerISR() to tick every M ms
void TimerSet(unsigned long M)
{
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

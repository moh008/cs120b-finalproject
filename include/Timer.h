#ifndef __Timer_h__
#define __Timer_h__

volatile unsigned char TimerFlag = 0;

void TimerOn();
void TimerOff();
void TimerISR();
ISR(TIMER1_COMPA_vect);
void TimerSet(unsigned long M);

#endif
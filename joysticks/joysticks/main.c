/*
 * joysticks.c
 *
 * Created: 2018-08-29 오전 5:19:24
 * Author : Lincoln
 */ 

#include <avr/io.h>
#include "io.c"
#include "Timer.c"

void ADC_init(){
	ADCSRA |= (1 << ADEN) | (1 <<ADSC) | (1<<ADATE);
}

enum lcd_States{display}lcd_state;
void writeLCD()
{
	unsigned short x;
	switch(lcd_state)
	{
		case display:
		lcd_state = display;
		break;
	}
	switch(lcd_state)
	{
		case display:
		ADC_init();
		x = ADC;
		if((x < 0x0100) && (x >= 0x0000))
		{
			LCD_Cursor(1);
			LCD_WriteData(1 + '0');
		}
		else if((x < 0x0200) && (x > 0x0100))
		{
			LCD_Cursor(1);
			LCD_WriteData(2 + '0');
		}
		else if((x < 0x0300) && (x > 0x0200))
		{
			LCD_Cursor(1);
			LCD_WriteData(3 + '0');
		}
		else if((x < 0x0400) && (x > 0x0300))
		{
			LCD_Cursor(1);
			LCD_WriteData(4 + '0');
		}
		break;
	}
}
int main(void)
{
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	
	unsigned long writeLCD_period = 1000;
	unsigned long writeLCD_elapsedTime = 0;
	const unsigned long timerPeriod = 1;
	
	LCD_init();
	LCD_ClearScreen();
	
	TimerSet(timerPeriod);
	TimerOn();
	
    while (1) 
    {
		if(writeLCD_period >= writeLCD_elapsedTime)
		{
			writeLCD();
			writeLCD_elapsedTime = 0;
		}
		while(!TimerFlag){};
		TimerFlag = 0;
		writeLCD_elapsedTime += timerPeriod;
    }
}
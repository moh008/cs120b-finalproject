#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "Timer.c"
#include "scheduler.h"
#include "io.c"
#include "shiftreg.h"

#define p1_up 0x01
#define p1_down 0x02
#define p2_up 0x04
#define p2_down 0x08
#define chooseleft 0x10
#define chooseright 0x20
#define recordcheck 0x40

#define read_eeprom_word(address) eeprom_read_word((const uint16_t*)address)
#define write_eeprom_word(address, value) eeprom_write_word((uint16_t*)address, (uint16_t)value)
#define update_eeprom_word(address, value) eeprom_update_word ((uint16_t*)address, (uint16_t)value)

//===============Global Variables===============
unsigned char EEMEM eeprom_array[6];
unsigned char p1_val[6] = {0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0};		//sets the pattern displayed on columns
unsigned char p1_sel = 0x7F;										//grounds column to display pattern
unsigned char p1_index = 0;												//p1 location index

unsigned char p2_val[6] = {0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0};
unsigned char p2_sel = 0xFE;
unsigned char p2_index = 0;											// p2 location index

unsigned char bot_val[6] = {0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0};
unsigned char bot_sel = 0xFE;
unsigned char bot_index = 0;										//bot location index

unsigned char single = 0;		// player1 vs bot
unsigned char multi = 0;		// player1 vs player2
unsigned char ready = 0;		// when game is ready to start
unsigned char write = 0;		// write on LCD screen
unsigned char finish = 0;		// when game finished
unsigned char diff = 0;

unsigned char p1_wins = 0;		//p1 wins, score flag
unsigned char p2_wins = 0;		//p2 wins, score flag
unsigned char p1_score = 0;		//p1 score cumulative, stored in EEPROM
unsigned char p2_score = 0;		//p2 score cumulative

unsigned char ball_val[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};	//ball pattern
unsigned char ball_sel[8] = {0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE};	//
unsigned char ball_index = 0;	//a pong ball pattern in a column
unsigned char ball_cindex = 0;	//ball column

unsigned char j = 0;

enum menu_States{home, tmp, difficulty, play, wait_score, write_p1, write_p2, p1_won_display, p2_won_display,
hold, tmp2, records, tmp3, records2, tmp4};
int menu(int state)
{//total rounds 5, if any player wins 3 first, return to home
	//===========Local variables==============
	unsigned char button = ~PINA & 0xF0;
	unsigned char wincount = 0;					//hold player won screen
	//========================================
	switch(state)
	{
		case home:
		if(button == chooseleft)
		{
			state = tmp;
			single = 1;
			multi = 0;
		}
		else if(button == chooseright)
		{
			state = tmp;
			single = 0;
			multi = 1;
		}
		else if(button == recordcheck)
		{
			state = tmp2;
		}
		else
		{
			state = home;
		}
		break;
		
		case tmp:
		if(button == chooseleft)
		{
			state = tmp;
		}
		else if(button == chooseright)
		{
			state = tmp;
		}
		else
		{
			state = difficulty;
		}
		break;
		
		case difficulty:
		if(button == chooseleft)
		{
			diff = 1;
			state = play;
		}
		else if(button == chooseright)
		{
			diff = 2;
			state = play;
		}
		else
		{
			state = difficulty;
		}
		break;
		
		case play:
		state = wait_score;
		break;
		
		case wait_score:
		if(p1_wins)
		{
			state = write_p1;
		}
		else if(p2_wins)
		{
			state = write_p2;
		}
		else if(!p1_wins && !p2_wins)
		{
			state = wait_score;
		}
		break;
		
		case write_p1:
		if(p1_score == 2)
		{
			state = p1_won_display;
		}
		else
		{
			state = wait_score;
		}
		break;
		
		case write_p2:
		if(p2_score == 2)
		{
			state = p2_won_display;
		}
		else
		{
			state = wait_score;
		}
		break;
		
		case p1_won_display:
		state = hold;
		break;
		
		case p2_won_display:
		state = hold;
		break;
		
		case hold:
		if(wincount == 20)
		{
			wincount = 0;
			state = wait_score;
			single = 0;
			multi = 0;
			ready = 0;
		}
		else
		{
			state = hold;
		}
		wincount++;
		break;
		
		case tmp2:
		if(button == recordcheck)
		{
			state = tmp2;
		}
		else
		{
			state = records;
		}
		break;
		
		case records:
		if(button == recordcheck)
		{
			state = records;
		}
		else
		{
			state = tmp3;
		}
		break;
		
		case tmp3:
		if(button == recordcheck)
		{
			state = records2;
		}
		else
		{
			state = tmp3;
		}
		break;
		
		case records2:
		if(button == recordcheck)
		{
			state = records2;
		}
		else
		{
			state = tmp4;
		}
		break;
		
		case tmp4:
		if(button == recordcheck)
		{
			state = home;
		}
		else
		{
			state = tmp4;
		}
		break;
		
		default:
		state = home;
		break;
	}
	switch(state)
	{
		case home:
		single = 0;
		multi = 0;
		p1_score = 0;
		p2_score = 0;
		finish = 0;
		ready = 0;
		diff = 0;
		LCD_DisplayString(1, "     Main     1p) 2p) records)");
		break;
		
		case tmp:
		break;
		
		case difficulty:
		LCD_DisplayString(1, "  Difficulty  Easy) Difficult)");
		break;
		
		case play:
		ready = 1;
		LCD_DisplayString(1, "     SCORE      P1)      P2)  ");
		break;
		
		case wait_score:
		break;
		
		case write_p1:
		LCD_Cursor(20);
		LCD_WriteData(p1_score + '0');
		break;
		
		case write_p2:
		LCD_Cursor(29);
		LCD_WriteData(p2_score + '0');
		break;
		
		case p1_won_display:
		single = 0;
		multi = 0;
		ready = 0;
		LCD_DisplayString(1, "    P1 Wins     ");
		write_eeprom_word(&eeprom_array[0], read_eeprom_word(&eeprom_array[0]) + 1);
		write_eeprom_word(&eeprom_array[3], read_eeprom_word(&eeprom_array[3]) + 1);
		break;
		
		case p2_won_display:
		single = 0;
		multi = 0;
		ready = 0;
		LCD_DisplayString(1, "    P2 Wins     ");
		write_eeprom_word(&eeprom_array[0], read_eeprom_word(&eeprom_array[0]) + 1);
		write_eeprom_word(&eeprom_array[5], read_eeprom_word(&eeprom_array[5]) + 1);
		break;
		
		case hold:
		single = 0;
		multi = 0;
		ready = 0;
		break;
		
		case tmp2:
		break;
		
		case records:	//display rounds played
		LCD_Cursor(1);
		LCD_DisplayString(1, "Rounds played: ");
		LCD_Cursor(15);
		LCD_WriteData(read_eeprom_word(&eeprom_array[0]) + '0');
		break;
		
		case tmp3:
		break;
		
		case records2:	//display score
		LCD_DisplayString(1, "P1)      P2)");
		LCD_Cursor(4);
		LCD_WriteData(read_eeprom_word(&eeprom_array[3]) + '0');
		LCD_Cursor(13);
		LCD_WriteData(read_eeprom_word(&eeprom_array[5]) + '0');
		break;
		
		case tmp4:
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
	DDRD = 0xFF; PORTD = 0x00;
	
	unsigned long int menu_period = 100;
	
	static task task1;
	task *tasks[] = {&task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = -1;
	task1.period = menu_period;
	task1.elapsedTime = menu_period;
	task1.TickFct = &menu;
	
	TimerSet(menu_period);
	TimerOn();
	LCD_init();
	LCD_ClearScreen();
	
	if(read_eeprom_word(&eeprom_array[0]) + '0' == '/')
	{
		write_eeprom_word(&eeprom_array[0], read_eeprom_word(&eeprom_array[0]) + 1);
	}
	if(read_eeprom_word(&eeprom_array[5]) + '0' == '/')
	{
		write_eeprom_word(&eeprom_array[5], read_eeprom_word(&eeprom_array[5]) + 1);
	}
	if(read_eeprom_word(&eeprom_array[3]) + '0' == '/')
	{
		write_eeprom_word(&eeprom_array[3], read_eeprom_word(&eeprom_array[3]) + 1);
	}
	
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
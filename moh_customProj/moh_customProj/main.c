/*
 * moh_pong project.c
 *
 * Created: 2018-08-25 오후 5:48:33
 * Author : Lincoln
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "Timer.c"
#include "scheduler.h"
#include "io.c"
#include "shiftreg.h"

#define p2_up 0x04
#define p2_down 0x08
#define chooseleft 0x10
#define chooseright 0x20
#define recordcheck 0x40

//===============Global Variables===============
unsigned char EEMEM eeprom_array[6];
unsigned char p1_val[6] = {0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0};		//sets the pattern displayed on columns
unsigned char p1_sel = 0x7F;										//grounds column to display pattern
unsigned char p1_index = 0;												//p1 location index

unsigned char p2_val[6] = {0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0};
unsigned char p2_sel = 0xFE;
unsigned char p2_index = 0;											// p2 location index

unsigned char single = 0;		// player1 vs bot
unsigned char multi = 0;		// player1 vs player2
unsigned char ready = 0;		// when game is ready to start
unsigned char write = 0;		// write on LCD screen
unsigned char diff = 0;

unsigned char p1_wins = 0;		//p1 wins, score flag
unsigned char p2_wins = 0;		//p2 wins, score flag
unsigned char p1_score = 0;		//p1 score cumulative, stored in EEPROM
unsigned char p2_score = 0;		//p2 score cumulative
	
unsigned char ball_val[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};	//ball pattern
unsigned char ball_sel[8] = {0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE};	//
unsigned char ball_index = 6;	//a pong ball pattern in a column
unsigned char ball_cindex = 6;	//ball column

int wall = -1;
int hit = -1;
unsigned char holdit = 0;////////////////////////////////////
unsigned char holdtime = 0;
unsigned char countdown = 0;

//==================================================================================

//==================Global functions=====================

//ADC control
void ADC_init(){
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1<<ADATE);
}

//EEPROM Macro
#define read_eeprom_word(address) eeprom_read_word((const uint16_t*)address)
#define write_eeprom_word(address, value) eeprom_write_word((uint16_t*)address, (uint16_t)value)
#define update_eeprom_word(address, value) eeprom_update_word ((uint16_t*)address, (uint16_t)value)
//==========================================================


unsigned char wincount = 0;					//hold player won screen
enum menu_States{home_init, home_wait, tmp, difficulty, wait_play, play, wait_score, write_p1, write_p2, p1_won_display, p2_won_display,
hold, tmp2, records, tmp3, records2, tmp4};
int menu(int state)
{//total rounds 5, if any player wins 3 first, return to home
	//===========Local variables==============
	unsigned char button = ~PINA & 0xF0;
	//========================================
	switch(state)
	{
		case home_init:
		if(button == recordcheck)
		{
			state = home_init;
		}
		else
		{
			state = home_wait;
		}
		break;
		
		case home_wait:
		if(button == chooseleft)
		{
			single = 1;
			multi = 0;
			state = tmp;
		}
		else if(button == chooseright)
		{
			single = 0;
			multi = 1;
			state = tmp;
		}
		else if(button == recordcheck)
		{
			state = tmp2;
		}
		else
		{
			single = 0;
			multi = 0;
			diff = 0;
			p1_score = 0;
			p2_score = 0;
			state = home_wait;
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
			diff = 2;	//makes ball speed 0.1ms/led
			state = play;
		}
		else if(button == chooseright)
		{
			diff = 1;	//speed 0.01ms/led
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
		if(p1_score == 3)
		{
			state = p1_won_display;
		}
		else
		{
			state = wait_score;
		}
		break;
		
		case write_p2:
		if(p2_score == 3)
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
		
		case hold:	//wait 5seconds for diaplying win message
		single = 0;
		multi = 0;
		ready = 0;
		if(wincount == 50)
		{
			wincount = 0;
			state = home_init;
		}
		else
		{
			state = hold;
		}
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
			state = home_init;
		}
		else
		{
			state = tmp4;
		}
		break;
		
		default:
		state = home_init;
		break;
	}
	switch(state)
	{
		case home_init:
		single = 0;
		multi = 0;
		ready = 0;
		p1_score = 0;
		p2_score = 0;
		diff = 0;
		LCD_ClearScreen();
		LCD_DisplayString(1, "     Main       1p) 2p) records)");
		break;
		
		case home_wait:
		break;
		
		case tmp:
		LCD_DisplayString(1, "   Difficulty   Easy)    insane)");
		break;
		
		case difficulty:
		break;
		
		case play:
		ready = 1;
		LCD_DisplayString(1, "     SCORE      P1)        P2)  ");
		break;
		
		case wait_score:
		/*LCD_ClearScreen();
		LCD_Cursor(1);
		LCD_WriteData(ball_cindex + '0');
		LCD_Cursor(4);
		LCD_WriteData(holdit + '0');
		LCD_Cursor(7);
		LCD_WriteData(holdtime+ '0');
		*/
		break;
		
		case write_p1:
		LCD_Cursor(20);
		LCD_WriteData(p1_score + '0');
		break;
		
		case write_p2:
		LCD_Cursor(31);
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
		wincount++;
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

//Display SM
enum display_States{DISPLAY};
int display(int state){
	//=============transitions==============
	switch(state){
		case DISPLAY:
		state = DISPLAY;
		break;
		
		default:
		state = DISPLAY;
		break;
	}
	//===============actions================
	switch(state){
		case DISPLAY:
		if(ready && single)
		{
			transmit_data(p1_sel, p1_val[p1_index]);
			transmit_data(p2_sel, p2_val[p2_index]);	
			transmit_data(ball_sel[ball_cindex], ball_val[ball_index]);
		}
		else if(ready && multi)
		{
			transmit_data(p1_sel, p1_val[p1_index]);
			transmit_data(p2_sel, p2_val[p2_index]);
			transmit_data(ball_sel[ball_cindex], ball_val[ball_index]);
		}
		else
		{
			transmit_data(p1_sel, p1_val[p1_index]);
			transmit_data(p2_sel, p2_val[p2_index]);
			transmit_data(ball_sel[ball_cindex], ball_val[ball_index]);
		}
		break;
		
		default:
		break;
	}
	return state;
}

//Move player1
enum p1_move_States{p1_init, p1_wait,p1_moveup, p1_dashup, p1_movedown, p1_dashdown};
int p1_move(int state)
{
	unsigned short x;
	ADC_init();
	x = ADC;
	switch(state)
	{//state transition
		case p1_init:
		state = p1_wait;
		break;
		
		case p1_wait:
		if((x < 0x0400) && (x > 0x03C0) && (p1_index != 4))
		{
			state = p1_dashup;
		}
		else if((x < 0x0080) && (x >= 0x0000) && (p1_index != 1))
		{
			state = p1_dashdown;
		}
		else if(((x < 0x03C0) && (x > 0x0280)) || ((x < 0x0400) && (x > 0x03C0) && (p1_index == 4)))
		{
			state = p1_moveup;
		}
		else if(((x < 0x0180) && (x > 0x0080)) || ((x < 0x0080) && (x >= 0x0000) && (p1_index == 1)))
		{
			state = p1_movedown;
		}
		else
		{
			state = p1_wait;
		}
		break;
		
		case p1_moveup:
		if((x < 0x03C0) && (x > 0x0280))
		{
			state = p1_moveup;
		}
		else if((x < 0x0400) && (x > 0x03C0) && (p1_index != 4))
		{
			state = p1_dashup;
		}
		else
		{
			state = p1_wait;
		}
		break;
		
		case p1_dashup:
		if(((x < 0x0400) && (x > 0x03C0)) && (p1_index != 4))
		{
			state = p1_dashup;
		}
		else if(((x < 0x03C0) && (x > 0x0280)) || ((x < 0x0400) && (x > 0x03C0) && (p1_index == 4)))
		{
			state = p1_moveup;
		}
		else
		{
			state = p1_wait;
		}
		break;
		
		case p1_movedown:
		if((x < 0x0180) && (x > 0x0080))
		{
			state = p1_movedown;
		}
		else if((x < 0x0080) && (x >= 0x0000) && (p1_index != 1))
		{
			state = p1_dashdown;
		}
		else
		{
			state = p1_wait;
		}
		break;
		
		case p1_dashdown:
		if(((x < 0x0180) && (x > 0x0080)) || ((x < 0x0080) && (x >= 0x0000) && (p1_index == 1)))
		{
			state = p1_movedown;
		}
		else if((x < 0x0080) && (x >= 0x0000) && (p1_index != 1))
		{
			state = p1_dashdown;
		}
		else
		{
			state = p1_wait;
		}
		break;
		
		
		default:
		state = p1_init;
		break;
	}
	switch(state)
	{//state actions
		case p1_init:
		break;
		
		case p1_wait:
		break;
		
		case p1_moveup:
		if(p1_index < 5)
		{
			p1_index++;
		}
		break;
		
		case p1_dashup:
		if(p1_index < 5)
		{
			p1_index += 2;
		}
		break;
		
		case p1_movedown:
		if(p1_index > 0)
		{
			p1_index--;
		}
		break;
		
		case p1_dashdown:
		if(p1_index > 0)
		{
			p1_index -= 2;
		}
		break;
		
		default:
		break;
	}
	return state;
}


//ball movement period 20ms
enum ballmove_States{ball_init, game_start, ball_wait, ball_move, hold_ball};
int ballmove(int state)
{
	switch(state)
	{
		case ball_init:
		if(ready)
		{
			state = game_start;
		}
		else if(!ready)
		{
			state = ball_init;
		}
		break;
		
		case game_start:
		if(ready)
		{
			state = ball_wait;
		}
		else if(!ready)
		{
			state = ball_init;
		}
		break;
		
		case ball_wait:
		if(countdown == 50)	//wait for 1seconds and begin the game
		{
			countdown = 0;
			state = ball_move;
		}
		else if(countdown < 50)
		{
			state = ball_wait;
		}
		break;
		
		case ball_move:
		{
			if(!ready)
			{
				state = ball_init;
			}
			else
			{
				state = hold_ball;
			}
		}
		break;
		
		case hold_ball:	//determine ball speed Ball_period: 100ms
		if(holdit < (diff*holdtime)) // if difficulty is difficult, diff=1, speed = 10. Then ball moves for each 1seconds
		{
			state = hold_ball;
			holdit += 1;
		}
		else if(holdit >= (diff*holdtime))
		{
			holdit = 0;
			if(p1_wins)
			{
				state = game_start;
			}
			else if(p2_wins)
			{
				state = game_start;
			}
			else
			{
				state = ball_move;
			}
		}
		break;
		
		default:
		state = ball_init;
		break;
	}
	switch(state)
	{
		case ball_init:
		p1_wins = 0;
		p2_wins = 0;
		holdit = 0;
		ready = 0;
		countdown = 0;
		break;
		
		case game_start://player options if either player wins
		if(!p1_wins && !p2_wins)	// initial settings
		{
			ball_cindex = 3;
			ball_index = 3;
			p1_index = 5;
			p2_index = 0;
			hit = -1;
			wall = 0;
			holdtime = 10;
		}
		else if(p1_wins)		// if p1 won
		{
			ball_cindex = 5;
			ball_index = 3;
			p1_index = 5;
			p2_index = 3;
			hit = 1;
			wall = 0;
			holdtime = 10;
		}
		else if(p2_wins)		// if p2 won
		{
			ball_cindex = 2;
			ball_index = 3;
			p1_index = 3;
			p2_index = 0;
			hit = -1;
			wall = 0;
			holdtime = 10;
		}
		break;
		
		case ball_wait:
		p1_wins = 0;
		p2_wins = 0;
		countdown++;
		break;
		
		case ball_move:
		countdown = 0;
		ball_cindex = ball_cindex + hit;
		ball_index = ball_index + wall;
		
		if(ball_index == 0 || ball_index == 7)
		{
			wall = wall * (-1);
		}
		else
		{
			if((ball_cindex == 1))
			{
				if(ball_index == 7)
				{
					if(p1_index == 5)
					{
						if(holdtime > 2) //ball speeds up when it hits the end of paddle, and change its direction
						{
							hit = 1;
							wall = -1;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = -1;
						}
					}
				}
				else if(ball_index == 6)
				{
					if(p1_index == 5)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = 0;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = 0;
						}
					}
					else if(p1_index == 4)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = 1;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = 1;
						}
					}
				}
				else if(ball_index == 5)
				{
					if(p1_index == 5)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = -1;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = -1;
						}
					}
					else if(p1_index == 4)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = 0;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = 0;
						}
					}
					else if(p1_index == 3)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = 1;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = 1;
						}
					}
				}
				else if(ball_index == 4)
				{
					if(p1_index == 4)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = -1;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = -1;
						}
					}
					else if(p1_index == 3)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = 0;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = 0;
						}
					}
					else if(p1_index == 2)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = 1;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = 1;
						}
					}
				}
				else if(ball_index == 3)
				{
					if(p1_index == 3)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = -1;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = -1;
						}
					}
					else if(p1_index == 2)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = 0;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = 0;
						}
					}
					else if(p1_index == 1)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = 1;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = 1;
						}
					}
				}
				else if(ball_index == 2)
				{
					if(p1_index == 2)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = -1;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = -1;
						}
					}
					else if(p1_index == 1)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = 0;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = 0;
						}
					}
					else if(p1_index == 0)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = 1;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = 1;
						}
					}
				}//
				else if(ball_index == 1)
				{
					if(p1_index == 1)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = -1;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = -1;
						}
					}
					else if(p1_index == 0)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = 0;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = 0;
						}
					}
				}
				else if(ball_index == 0)
				{
					if(p1_index == 0)
					{
						if(holdtime > 2)
						{
							hit = 1;
							wall = 1;
							holdtime--;
						}
						else
						{
							hit = 1;
							wall = -1;
						}
					}
				}
			}
			else if((ball_cindex == 6))
			{
				if(ball_index == 7)
				{
					if(p2_index == 5)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = -1;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = -1;
						}
					}
				}
				else if(ball_index == 6)
				{
					if(p2_index == 5)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = 0;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = 0;
						}
					}
					else if(p2_index == 4)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = 1;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = 1;
						}
					}
				}
				else if(ball_index == 5)
				{
					if(p2_index == 5)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = -1;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = -1;
						}
					}
					else if(p2_index == 4)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = 0;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = 0;
						}
					}
					else if(p2_index == 3)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = 1;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = 1;
						}
					}
				}
				//ball4
				else if(ball_index == 4)
				{
					if(p2_index == 4)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = -1;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = -1;
						}
					}
					else if(p2_index == 3)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = 0;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = 0;
						}
					}
					else if(p2_index == 2)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = 1;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = 1;
						}
					}
				}
				//ballindex 3
				else if(ball_index == 3)
				{
					if(p2_index == 3)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = -1;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = -1;
						}
					}
					else if(p2_index == 2)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = 0;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = 0;
						}
					}
					else if(p2_index == 1)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = 1;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = 1;
						}
					}
				}
				//ballindex2
				else if(ball_index == 2)
				{
					if(p2_index == 2)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = -1;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = -1;
						}
					}
					else if(p2_index == 1)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = 0;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = 0;
						}
					}
					else if(p2_index == 0)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = 1;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = 1;
						}
					}
				}
				//ballindex1
				else if(ball_index == 1)
				{
					if(p2_index == 1)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = -1;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = -1;
						}
					}
					else if(p2_index == 0)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = 0;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = 0;
						}
					}
				}
				//ballindex = 0
				else if(ball_index == 0)
				{
					if(p2_index == 0)
					{
						if(holdtime > 2)
						{
							hit = -1;
							wall = 1;
							holdtime--;
						}
						else
						{
							hit = -1;
							wall = 1;
						}
					}
				}
			}
			else if(ball_cindex == 0)
			{
				p2_wins = 1;
				p2_score++;
			}
			else if(ball_cindex == 7)
			{
				p1_wins = 1;
				p1_score++;
			}
		}
		break;
		
		case hold_ball:
		break;
		
		default:
		break;
	}
	return state;
}


//Move player2
enum p2_move_States{p2_init, p2_wait, p2_moveup, p2_movedown};
int p2_move(int state)
{
	unsigned char button = ~PINA & 0x0C;
	switch(state)
	{//state transition
		case p2_init:
		if(!ready && !multi)
		{
			state = p2_init;
		}
		else if(ready && multi)
		{
			state = p2_wait;
		}
		else if(!ready && single)
		{
			state = p2_init;
		}
		break;
		
		case p2_wait:
		if(button == p2_up)
		{
			state = p2_moveup;
		}
		else if(button == p2_down)
		{
			state = p2_movedown;
		}
		else
		{
			state = p2_wait;
		}
		break;
		
		case p2_moveup:
		if(button == p2_up)
		{
			state = p2_moveup;
		}
		else
		{
			state = p2_wait;
		}
		break;
		
		case p2_movedown:
		if(button == p2_down)
		{
			state = p2_movedown;
		}
		else
		{
			state = p2_wait;
		}
		break;
		
		default:
		state = p2_init;
		break;
	}
	switch(state)
	{//state actions
		case p2_init:
		break;
		
		case p2_wait:
		break;
		
		case p2_moveup:
		if(p2_index < 5)
		{
			p2_index++;
		}
		break;
		
		case p2_movedown:
		if(p2_index > 0)
		{
			p2_index--;
		}
		break;

		default:
		break;
	}
	return state;
}

enum bot_move_States{bot_wait, bot_ready, bot_moveup, bot_movedown};
int bot_move(int state)
{
	switch(state)
	{
		case bot_wait:
		if(ready && single && !multi)
		{
			state = bot_ready;
		}
		else if(!ready && !single)
		{
			state = bot_wait;
		}
		else
		{
			state = bot_wait;
		}
		break;
		
		case bot_ready:
		if(!ready)
		{
			state = bot_wait;
		}
		else if(ready && single)
		{
			if(ball_index < p2_index)
			{
				state = bot_movedown;
			}
			else if(ball_index > p2_index)
			{
				state = bot_moveup;
			}
			else
			{
				state = bot_ready;
			}
		}
		break;
		
		case bot_moveup:
		state = bot_ready;
		break;
		
		case bot_movedown:
		state = bot_ready;
		break;
		
		default:
		state = bot_wait;
		break;
	}
	switch(state)
	{
		case bot_wait:
		break;
		
		case bot_ready:
		break;
		
		case bot_moveup:
		if(ball_index < 5)
		{
			p2_index++;
		}
		break;
		
		case bot_movedown:
		if(ball_index > 0)
		{
			p2_index--;
		}
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
	
	unsigned short menu_period = 100;
	unsigned short display_period = 1;	//Display Matrix period
	unsigned short p1_move_period = 200;	//P1 move period
	unsigned short ballmove_period = 20;	//ball move period
	unsigned short p2_move_period = 100;	//P2 move period
	unsigned short bot_move_period = 300;
	
	static task task1, task2, task3, task4, task5, task6;
	task *tasks[] = {&task1, &task2, &task3, &task4, &task5, &task6};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = -1;
	task1.period = menu_period;
	task1.elapsedTime = menu_period;
	task1.TickFct = &menu;
	
	task2.state = -1;
	task2.period = p1_move_period;
	task2.elapsedTime = p1_move_period;
	task2.TickFct = &p1_move;
	
	task3.state = -1;
	task3.period = ballmove_period;	//ball move period. Can we make this faster? or make it intentionally slow then make faster?
	task3.elapsedTime = ballmove_period;
	task3.TickFct = &ballmove;
	
	task4.state = -1;
	task4.period = p2_move_period;
	task4.elapsedTime = p2_move_period;
	task4.TickFct = &p2_move;
	
	task5.state = -1;
	task5.period = bot_move_period;
	task5.elapsedTime = bot_move_period;
	task5.TickFct = &bot_move;

	task6.state = -1;
	task6.period = display_period;
	task6.elapsedTime = display_period;
	task6.TickFct = &display;
	
	LCD_init();
	TimerSet(1);
	TimerOn();
	
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
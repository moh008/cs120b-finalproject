/*
 * moh008_lab10_part3.c
 *
 * Created: 2018-02-23 오후 9:01:19
 * Author : Lincoln
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "bit.h"
#include "io.c"
#include "timer.c"
// Returns '\0' if no key pressed, else returns char '1', '2', ... '9', 'A', ...
// If multiple keys pressed, returns leftmost-topmost one
// Keypad must be connected to port C
/* Keypad arrangement
        PC4 PC5 PC6 PC7
   col  1   2   3   4
row
PC0 1   1 | 2 | 3 | A
PC1 2   4 | 5 | 6 | B
PC2 3   7 | 8 | 9 | C
PC3 4   * | 0 | # | D
*/
unsigned char GetKeypadKey() {

	PORTC = 0xEF; // Enable col 1 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('1'); }
	if (GetBit(PINC,1)==0) { return('4'); }
	if (GetBit(PINC,2)==0) { return('7'); }
	if (GetBit(PINC,3)==0) { return('*'); }

	// Check keys in col 2
	PORTC = 0xDF; // Enable col 2 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC, 0) == 0) { return ('2'); }
	if (GetBit(PINC, 1) == 0) { return ('5'); }
	if (GetBit(PINC, 2) == 0) { return ('8'); }
	if (GetBit(PINC, 3) == 0) { return ('0'); }
	// ... *****FINISH*****

	// Check keys in col 3
	PORTC = 0xBF; // Enable col 3 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if(GetBit(PINC, 0) == 0) { return ('3'); }
	if(GetBit(PINC, 1) == 0) { return ('6'); }
	if(GetBit(PINC, 2) == 0) { return ('9'); }
	if(GetBit(PINC, 3) == 0) { return ('#'); }
	// ... *****FINISH*****

	// Check keys in col 4
	PORTC = 0x7F;	// Enable col 4 with 0, disable others with 1's
	asm("nop");		// add a delay to allow PORTC to stabilize before checking
	if(GetBit(PINC, 0) == 0) { return ('A');}
	if(GetBit(PINC, 1) == 0) { return ('B');}
	if(GetBit(PINC, 2) == 0) { return ('C');}
	if(GetBit(PINC, 3) == 0) { return ('D');}
	// ... *****FINISH*****

	return('\0'); // default value

}
//--------Find GCD function --------------------------------------------------
unsigned long int findGCD( unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while (1){
		c = a%b;
		if (c==0){ return b;}
		a = b;
		b = c;
	}
	return 0;
}
//--------End find GCD function ----------------------------------------------
//--------Task scheduler data structure---------------------------------------
//Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
	/*Tasks should have members that include: state, period,
	a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)( int ); //Task tick function
} task;
//--------End Task scheduler data structure-----------------------------------
//--------Shared Variables----------------------------------------------------
unsigned char x = 0;
//--------End Shared Variables------------------------------------------------

//--------User defined FSMs---------------------------------------------------

//Enumeration of states.

enum SM1_States {SM1_readkeypad};	
int SMTick1 (int state)
{
	switch(state)
	{
		case SM1_readkeypad:
		break;
		
		default:
		state = SM1_readkeypad;
		break;
	}
	switch(state)
	{
		case SM1_readkeypad:
		x = GetKeypadKey();
		
		default:
		break;
	}
	return x;
}
enum SM2_States{SM2_wait, SM2_writeLCD, SM2_rlsd};
int SMTick2(int state)
{
	switch(state)
	{
		case SM2_wait:
		if((x == '1') || (x == '2') || (x == '3') || (x == '4') 
		|| (x == '5') || (x == '6') || (x == '7') || (x == '8') 
		|| (x == '9') || (x == 'A') || (x == 'B') || (x == 'C') 
		|| (x == 'D') || (x == '*') || (x == '0') || (x == '#'))
		{
			state = SM2_writeLCD;
		}
		else 
		{
			break;
		}
		break;
		
		case SM2_writeLCD:
		state = SM2_rlsd;
		break;
		
		case SM2_rlsd:
		if((x == '1') || (x == '2') || (x == '3') || (x == '4')
		|| (x == '5') || (x == '6') || (x == '7') || (x == '8')
		|| (x == '9') || (x == 'A') || (x == 'B') || (x == 'C')
		|| (x == 'D') || (x == '*') || (x == '0') || (x == '#'))
		{
			state = SM2_rlsd;
		}
		else
		{
			state = SM2_wait;
		}
		
		default:
		state = SM2_wait;
		break;
	}
	
	switch(state)
	{
		case SM2_wait:
		break;
		
		case SM2_writeLCD:
		switch(x)
		{
			case '1': LCD_WriteData(1+'0'); LCD_Cursor(1); break; // hex equivalent
			case '2': LCD_WriteData(2+'0'); LCD_Cursor(1);break ;
			case '3': LCD_WriteData(3+'0'); LCD_Cursor(1);break ;
			case '4': LCD_WriteData(4+'0'); LCD_Cursor(1);break ;
			case '5': LCD_WriteData(5+'0'); LCD_Cursor(1);break ;
			case '6': LCD_WriteData(6+'0'); LCD_Cursor(1);break ;
			case '7': LCD_WriteData(7+'0'); LCD_Cursor(1);break ;
			case '8': LCD_WriteData(8+'0'); LCD_Cursor(1);break ;
			case '9': LCD_WriteData(9+'0'); LCD_Cursor(1);break ;
			case 'A': LCD_WriteData('A'); LCD_Cursor(1);break ;
			case 'B': LCD_WriteData('B'); LCD_Cursor(1);break ;
			case 'C': LCD_WriteData('C'); LCD_Cursor(1);break ;
			case 'D': LCD_WriteData('D'); LCD_Cursor(1);break ;
			case '*': LCD_WriteData('*'); LCD_Cursor(1);break ;
			case '0': LCD_WriteData(0+'0'); LCD_Cursor(1);break ;
			case '#': LCD_WriteData('#'); LCD_Cursor(1);break ;
			default : break ; // Should never occur. Middle LED off.
		}
		
		case SM2_rlsd:
		break;
		
		default:
		break;
	}
	return 0;
}

// --------END User defined FSMs-----------------------------------------------
// Implement scheduler code from PES.
int main()
{
	// Set Data Direction Registers
	// Buttons PORTA[0-7], set AVR PORTA to pull down logic
	DDRA = 0xFF; PORTA = 0x00;
	DDRD = 0xFF; PORTD = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	LCD_init();
	// . . . etc
	// Period for the tasks
	unsigned long int SMTick1_calc = 10;
	unsigned long int SMTick2_calc = 50;
	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;
	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;
	//Declare an array of tasks
	static task task1, task2;
	task *tasks[] = {&task1, &task2};
	const unsigned short numTasks = sizeof (tasks)/ sizeof (task*);
	// Task 1
	task1.state = -1; //Task initial state.
	task1.period = SMTick1_period; //Task Period.
	task1.elapsedTime = SMTick1_period; //Task current elapsed time.
	task1.TickFct = &SMTick1; //Function pointer for the tick.
	// Task 2
	task2.state = -1; //Task initial state.
	task2.period = SMTick2_period; //Task Period.
	task2.elapsedTime = SMTick2_period; //Task current elapsed time.
	task2.TickFct = &SMTick2; //Function pointer for the tick.
	
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	unsigned short i; // Scheduler for-loop iterator
	
	
	
	while (1) {
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while (!TimerFlag);
		TimerFlag = 0;
	}
	// Error: Program should not exit!
	return 0;
}
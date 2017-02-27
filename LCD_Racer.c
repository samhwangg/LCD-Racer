/*
 * shwan022_CS120B_FinalProject.c
 *
 * Created: 9/8/2015 5:48:59 PM
 *  Author: Samuel
 */ 


#include <avr/io.h>
#include <stdlib.h>
#include "/users/Samuel/Downloads/includes/bit.h"
#include "/users/Samuel/Downloads/includes/io.c"
#include "/users/Samuel/Downloads/includes/timer.h"

/*
Samuel Hwang's CS120B Project (Switzerland Summer Study Abroad Program 2015)
Professor: Philip Brisk
TA: Jeffery McDaniels

Instructions: The game opens with the main menu. There are two options possible from this screen:
1) Press the button D7 will start one player mode. 
2) Press the 'D' on the keypad will start two player mode.

During the game, the cursor can move up and down using D1 (up) and D4 (down) to avoid obstacles '@', '#', and '|'.
It is possible to shoot and destroy '|' if the player presses the missile button D3. The missile will launch across the screen and destroy '|'.
As time progresses, the game will move at a much faster pace. Everything, the car, obstacles, and the missiles, will move faster.
The game ends when the cursor touches any obstacle. The "Game Over!" screen flashes for two seconds and displays the score.
Score is accumulated by the amount of blocks passed.

From the score screen there are two options:
1) Pressing the button D7 will return to the main menu.
2) Pressing the up or down (D1 or D4 respectively) will restart the game from whatever setting it was on before. (1 player or 2 players)

Two player mode uses both the buttons connected to PIND and the keypad connected to PINC.
The objective of player two is to shoot missiles to throw off player one. But, the missiles from player two move six times slower.
If player two's missiles connect, player 1 loses and the screen goes to the "Game Over!" screen and then the score screen. 
Like one player mode, there are two options from the score screen:
1) Pressing D7 will return to the main menu. 
2) Pressing the up or down (D1 or D4 respectively) will restart the game from whatever setting it was on before. (1 player or 2 players) 
*/

unsigned char GetKeypadKey() {
	// Check keys in col 1
	// Enable col 4 with 0, disable others with 1’s
	// The delay allows PORTC to stabilize before checking
	PORTC = 0xEF;
	asm("nop");
	if (GetBit(PINC,0)==0) { return('1'); }
	if (GetBit(PINC,1)==0) { return('4'); }
	if (GetBit(PINC,2)==0) { return('7'); }
	if (GetBit(PINC,3)==0) { return('*'); }
	// Check keys in col 2
	// Enable col 5 with 0, disable others with 1’s
	// The delay allows PORTC to stabilize before checking
	PORTC = 0xDF;
	asm("nop");
	if (GetBit(PINC,0)==0) { return('2'); }
	if (GetBit(PINC,1)==0) { return('5'); }
	if (GetBit(PINC,2)==0) { return('8'); }
	if (GetBit(PINC,3)==0) { return('0'); }
	// Check keys in col 3
	// Enable col 6 with 0, disable others with 1’s
	// The delay allows PORTC to stabilize before checking
	PORTC = 0xBF;
	asm("nop");
	if (GetBit(PINC,0)==0) { return('3'); }
	if (GetBit(PINC,1)==0) { return('6'); }
	if (GetBit(PINC,2)==0) { return('9'); }
	if (GetBit(PINC,3)==0) { return('#'); }
	// Check keys in col 4
	PORTC = 0x7F;
	if (GetBit(PINC,0)==0) { return('A'); }
	if (GetBit(PINC,1)==0) { return('B'); }
	if (GetBit(PINC,2)==0) { return('C'); }
	if (GetBit(PINC,3)==0) { return('D'); }
	return('\0'); // default value
}

unsigned long int findGCD (unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1)
	{
		c = a%b;
		if(c == 0){return b;}
		a = b;
		b = c;
	}
	return 0;
}

typedef struct _task
{
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;

	int (*TickFct)(int);
} task;

//Global Variables
unsigned char x = '\0';
unsigned char stringStart[30] = "Sam's CS120B    Final Project";
unsigned char stringEnd[11] = "Game Over!";
unsigned char stringScore[8] = "Score:";
unsigned char cursorPosition = 1;
unsigned char cursorPosition2 = 16;
unsigned char obstacle1Position = 0;
unsigned char obstacle2Position = 0;
unsigned char obstacle3Position = 0;
unsigned char obstacle4Position = 0;
unsigned char bulletPosition = 0;
unsigned char bulletHit3 = 10;
unsigned char bulletHit4 = 10;
unsigned char laserPosition = 0;
unsigned long holdDisplayCount = 0;
unsigned char noInputCount = 0;
unsigned char speedControl = 4;
unsigned char speed = 0;
unsigned long score = 0;
unsigned char showScore = 0;
unsigned char showScorePosition;
unsigned char randomNumber;
unsigned long slowDown = 0;
unsigned char laserCount = 3;
unsigned long holdCount = 0;
unsigned short player2option = 0;

enum gameState{start, displayString1, gameStart, gameOver, displayString2, displayScore, holdStart, holdGame} display;

int SMTick1 (int display)
{
	unsigned char button = (~PIND) & 0x80;
	unsigned char up = (~PIND) & 0x02;
	unsigned char down = (~PIND) & 0x10;
	x = GetKeypadKey();
	switch(display)
	{
		case start:
		LCD_DisplayString(1, stringStart);
		LCD_Cursor(0);
		player2option = 0;
		display = displayString1;
		break;
		
		case displayString1:
		if(button)
		{
			LCD_ClearScreen();
			display = gameStart;
		}
		else if(x == 'D')
		{
			LCD_ClearScreen();
			player2option = 1;
			display = gameStart;
		}
		else
		{
			display = displayString1;
		}
		break;

		case gameStart:
		if(cursorPosition == obstacle1Position || cursorPosition == obstacle2Position || cursorPosition == obstacle3Position || cursorPosition == obstacle4Position || laserPosition == cursorPosition)
		{
			display = displayString2;
		}
		else
		{
			LCD_Cursor(cursorPosition);
			if(up)
			{
				cursorPosition = 1;
			}
			else if(down)
			{
				cursorPosition = 17;
			}
		}
		break;
		
		case displayString2:
		if(holdDisplayCount < 500)
		{
			holdDisplayCount++;
		}
		else
		{
			holdDisplayCount = 0;
			LCD_ClearScreen();
			LCD_DisplayString(1, stringEnd);
			display = gameOver;
		}
		break;

		case gameOver:
		if(holdDisplayCount < 2000)
		{
			holdDisplayCount++;
		}
		else
		{
			showScorePosition = 7;
			LCD_DisplayString(1, stringScore);
			while(score > 100)
			{
				showScore++;
				score = score - 100;
			}
			if(showScore != 0)
			{
				LCD_Cursor(showScorePosition);
				LCD_WriteData(showScore + '0');
				showScore = 0;
				showScorePosition++;
			}
			
			while(score > 10)
			{
				showScore++;
				score = score - 10;
			}
			LCD_Cursor(showScorePosition);
			LCD_WriteData(showScore + '0');
			showScore = 0;
			showScorePosition++;
			
			while(score > 1)
			{
				showScore++;
				score = score - 1;
			}
			LCD_Cursor(showScorePosition);
			LCD_WriteData(showScore + '0');
			showScore = 0;
			showScorePosition++;
			
			score = 0;
			display = displayScore;
			holdDisplayCount = 0;
			cursorPosition = 1;
		}
		break;
		
		case displayScore:
		if(holdDisplayCount < 2000)
		{
			holdDisplayCount++;
		}
		else if(up || down)
		{
			holdDisplayCount = 0;
			display = holdGame;
		}
		else if(button)
		{
			holdDisplayCount = 0;
			display = holdStart;
		}
		else
		{
			display = displayScore;
		}
		break;
		
		case holdStart:
		if(button)
		{
			display = holdStart;
		}
		else
		{
			display = start;
		}
		break;
		
		case holdGame:
		if(up || down)
		{
			display = holdGame;
		}
		else
		{
			display = gameStart;
		}
		break;
		
		default:
		display = start;
		break;

	}
	return display;
}

enum opponentAction{begin, movement, hold, holdStart2, holdGame2} position;

int SMTick2 (int position)
{
	unsigned char button2 = (~PIND) & 0x80;
	unsigned char up2 = (~PIND) & 0x02;
	unsigned char down2 = (~PIND) & 0x10;
	x = GetKeypadKey();
	switch(position)
	{
		case begin:
		if(x == 'D')
		{
			position = movement;
		}
		else
		{
			position = begin;
		}
		break;
		
		case movement:
		if(cursorPosition == obstacle1Position || cursorPosition == obstacle2Position || cursorPosition == obstacle3Position || cursorPosition == obstacle4Position || laserPosition == cursorPosition)
		{
			slowDown = 0;
			laserCount = 3;
			cursorPosition2 = 16;
			position = hold;
		}
		//to move opponent up and down
		if(x == '5')
		{
			cursorPosition2 = 16;
		}
		else if(x == '8')
		{
			cursorPosition2 = 32;
		}
		//to read in laser input (ONLY HAS 3 LASERS)
		if(x == '*')
		{
			if(!laserPosition && laserCount)
			{
				if(cursorPosition2 == 16)
				{
					laserPosition = 15;
					laserCount--;
				}
				else if(cursorPosition2 == 32)
				{
					laserPosition = 31;
					laserCount--;
				}
			}
		}
		break;
		
		case hold:
		if(holdCount < 4500)
		{
			holdCount++;
		}
		else if(up2 || down2)
		{
			position = holdGame2;
			holdCount = 0;
		}
		else if(button2)
		{
			position = holdStart2;
			holdCount = 0;
		}
		else
		{
			laserPosition = 0;
			position = hold;
		}
		break;
		
		case holdStart2:
		if(button2)
		{
			position = holdStart2;
		}
		else
		{
			position = begin;
		}
		break;
		
		case holdGame2:
		if(up2 || down2)
		{
			position = holdGame2;
		}
		else
		{
			position = movement;
		}
		break;
		
		default:
		position = begin;
		break;
		
	}
	return position;
}

enum moveObstacles{init, startMove, explosion, noInput, holdStart3, holdGame3} state;

int SMTick3(int state)
{
	unsigned char button3 = (~PIND) & 0x80;
	unsigned char up3 = (~PIND) & 0x02;
	unsigned char down3 = (~PIND) & 0x10;
	unsigned char bullet = (~PIND) & 0x08;
	x = GetKeypadKey();
	switch(state)
	{
		case init:
		if(button3 || x == 'D')
		{
			state = startMove;
		}
		else
		{
			state = init;
		}
		break;

		case startMove:
		if(speedControl <= 0)
		{
			randomNumber = rand() % 15 + 1;
			if(cursorPosition == obstacle1Position || cursorPosition == obstacle2Position || cursorPosition == obstacle3Position || cursorPosition == obstacle4Position || cursorPosition == laserPosition)
			{
				speed = 0;
				speedControl = 4;
				obstacle1Position = 0;
				obstacle2Position = 0;
				obstacle3Position = 0;
				obstacle4Position = 0;
				bulletPosition = 0;
				state = explosion;
			}
			else
			{
				LCD_ClearScreen();
				LCD_Cursor(cursorPosition); //buffer

				//bullet projectile if one does not exist
				if(bullet)
				{
					if(!bulletPosition)
					{
						if(cursorPosition == 17)
						{
							bulletPosition = 18;
						}
						else
						{
							bulletPosition = 2;
						}
					}
				}

				//releases obstacles
				if(obstacle1Position == 0 && obstacle3Position == 0)
				{
					if(randomNumber <= 6 && obstacle2Position <= 27)
					{
						obstacle1Position = 16;
					}
					else if(randomNumber == 7)
					{
						obstacle3Position = 16;
					}
				}
				if(obstacle2Position == 0 && obstacle4Position == 0)
				{
					if(randomNumber >= 9 && obstacle1Position <= 12)
					{
						obstacle2Position = 32;
					}
					
					else if(randomNumber == 8)
					{
						obstacle4Position = 32;
					}
				}
				
				//display objects
				LCD_Cursor(obstacle1Position);
				LCD_WriteData('@');
				LCD_Cursor(obstacle2Position);
				LCD_WriteData('#');
				LCD_Cursor(obstacle3Position);
				LCD_WriteData('|');
				LCD_Cursor(obstacle4Position);
				LCD_WriteData('|');
				
				//display opponent
				if(player2option) {
				LCD_Cursor(cursorPosition2);
				LCD_WriteData('*');
				LCD_Cursor(laserPosition);
				LCD_WriteData('<');
				}
				
				//display bullet
				LCD_Cursor(bulletPosition);
				LCD_WriteData('>');
				
				if(bulletPosition)
				{
					if(bulletPosition < 15)
					{
						bulletPosition++;
					}
					else if(bulletPosition < 31 && bulletPosition > 17)
					{
						bulletPosition++;
					}
					else
					{
						bulletPosition = 0;
					}
					
					//animation when bullet hits obstacle 3 or 4
					if(obstacle3Position)
					{
						bulletHit3 = obstacle3Position - bulletPosition;
						if(bulletHit3 <= 1)
						{
							LCD_Cursor(obstacle3Position);
							LCD_WriteData('\0');
							obstacle3Position = 0;
							bulletPosition = 0;
						}
					}
					if(obstacle4Position)
					{
						bulletHit4 = obstacle4Position - bulletPosition;
						if(bulletHit4 <= 1)
						{
							LCD_Cursor(obstacle4Position);
							LCD_WriteData('\0');
							obstacle4Position = 0;
							bulletPosition = 0;
						}
					}
				}

				//moves obstacles and toggles speed
				if(obstacle1Position > 1)
				{
					obstacle1Position--;
					if(obstacle1Position == 1)
					{
						speed++;
					}
				}
				else
				{
					obstacle1Position = 0;
				}
				if(obstacle2Position > 17)
				{
					obstacle2Position--;
					if(obstacle2Position == 17)
					{
						speed++;
					}
				}
				else
				{
					obstacle2Position = 0;
				}
				if(obstacle3Position > 1)
				{
					obstacle3Position--;
					if(obstacle3Position == 1)
					{
						speed++;
					}
				}
				else
				{
					obstacle3Position = 0;
				}
				if(obstacle4Position > 17)
				{
					obstacle4Position--;
					if(obstacle4Position == 17)
					{
						speed++;
					}
				}
				else
				{
					obstacle4Position = 0;
				}
				
				if(slowDown >= 6)
				{
					if(laserPosition)
					{
						if(laserPosition > 17)
						{
							laserPosition--;
						}
						else if(laserPosition == 16)
						{
							laserPosition = 0;
						}
						else if(laserPosition > 1)
						{
							laserPosition--;
						}
						else
						{
							laserPosition = 0;
						}
					}
					slowDown = 0;
				}
				else
				{
					slowDown++;
				}
				//toggles speed of objects
				if(speed < 2)
				{
					speedControl = 3;
				}
				else if(speed < 5)
				{
					speedControl = 2;
				}
				else if(speed < 7)
				{
					speedControl = 1;
				}
				else
				{
					speedControl = 0;
				}
				score++;
			}
		}
		else
		{
			speedControl--;
		}
		break;

		case explosion:
		if(cursorPosition == 1)
		{
			for(cursorPosition = 1; cursorPosition <= 8; cursorPosition++)
			{
				LCD_Cursor(cursorPosition);
				LCD_WriteData('\0');
				LCD_Cursor(cursorPosition + 16);
				LCD_WriteData('\0');
			}
			state = noInput;
		}
		else
		{
			for(cursorPosition = 17; cursorPosition <= 24; cursorPosition++)
			{
				LCD_Cursor(cursorPosition);
				LCD_WriteData('\0');
				LCD_Cursor(cursorPosition - 16);
				LCD_WriteData('\0');
			}
			state = noInput;
		}
		break;

		case noInput:
		if(noInputCount < 90)
		{
			noInputCount++;
		}
		else if(up3 || down3)
		{
			state = holdGame3;
			noInputCount = 0;
		}
		else if(button3)
		{
			state = holdStart3;
			noInputCount = 0;
		}
		else
		{
			state = noInput;
		}
		break;
		
		case holdStart3:
		if(button3)
		{
			state = holdStart3;
		}
		else
		{
			state = init;
		}
		break;
		
		case holdGame3:
		if(up3 || down3)
		{
			state = holdGame3;
		}
		else
		{
			state = startMove;
		}
		break;
		
		default:
		state = init;
		break;
	}
	return state;
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00; //data lines
	DDRB = 0xFF; PORTB = 0x00; //control lines
	DDRC = 0xF0; PORTC = 0x0F; //keypad input
	DDRD = 0x00; PORTD = 0xFF; //player1

	unsigned long int SMTick1_calc = 1;
	unsigned long int SMTick2_calc = 1;
	unsigned long int SMTick3_calc = 50;

	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);

	unsigned long int GCD = tmpGCD;

	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;
	unsigned long int SMTick3_period = SMTick3_calc/GCD;

	static task task1, task2, task3;
	task *tasks[] = {&task1, &task2, &task3};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	task1.state = -1;
	task1.period = SMTick1_period;
	task1.elapsedTime = SMTick1_period;
	task1.TickFct = &SMTick1;

	task2.state = -1;
	task2.period = SMTick2_period;
	task2.elapsedTime = SMTick2_period;
	task2.TickFct = &SMTick2;

	task3.state = -1;
	task3.period = SMTick3_period;
	task3.elapsedTime = SMTick3_period;
	task3.TickFct = &SMTick3;

	LCD_init();

	TimerSet(GCD);
	TimerOn();
	
	srand(300);
	
	unsigned short i;
	while(1) {
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime ==
			tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state =
				tasks[i]->TickFct(tasks[i]->state);
				// Reset elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	// Error: Program should not exit!
	return 0;

}
LCD Racer (Switzerland Summer Study Abroad Program 2015)
==============
Programmer Input/Output:
PORTA - AVR Port A6 & A7
PORTB - AVR PORT B0-B7
PORTC - Keypad input
		5 - moves player 2 up
		8 - moves player 2 down
		* - player 2 shoots projectile
PORTD - Button Input
		D1 - moves player 1 up
		D4 - moves player 1 down
		D3 - player 1 shoots projectile
		D7 - start button

<h1> Instructions </h1>
<hr>
The game opens with the main menu. There are two options possible from this screen:
1. Press the button D7 will start one player mode. 
2. Press the 'D' on the keypad will start two player mode.

- During the game, the cursor can move up and down using D1 (up) and D4 (down) to avoid obstacles '@', '#', and '|'.
- It is possible to shoot and destroy '|' if the player presses the missile button D3. The missile will launch across the screen and destroy '|'.
- As time progresses, the game will move at a much faster pace. Everything, the car, obstacles, and the missiles, will move faster.
- The game ends when the cursor touches any obstacle. The "Game Over!" screen flashes for two seconds and displays the score.
- Score is accumulated by the amount of blocks passed.

From the score screen there are two options:
1. Pressing the button D7 will return to the main menu.
2. Pressing the up or down (D1 or D4 respectively) will restart the game from whatever setting it was on before. (1 player or 2 players)

Two player mode uses both the buttons connected to PIND and the keypad connected to PINC.

The objective of player two is to shoot missiles to throw off player one. But, the missiles from player two move six times slower.
If player two's missiles connect, player 1 loses and the screen goes to the "Game Over!" screen and then the score screen. 
Like one player mode, there are two options from the score screen:
1) Pressing D7 will return to the main menu. 
2) Pressing the up or down (D1 or D4 respectively) will restart the game from whatever setting it was on before. (1 player or 2 players) 
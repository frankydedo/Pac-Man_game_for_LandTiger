/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"
#include "../led/led.h"
#include "GLCD/GLCD.h"
#include "stdio.h"
#include "../shared.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/


extern char remainingTime[];
extern int timer;
extern int pause;
extern int pause;
extern int nextInput;
extern int countPill;
extern uint16_t empty_life[];
int temp;
int decrease_lives=1;

//immagine di gamoeover
extern uint16_t ending_image[];
extern uint32_t ghost_speed;

uint16_t SinTable[45] = {
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

//bocca chiusa
uint16_t pacman0 [] = {
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 
0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000, 0xffe0, 
0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 
0xffe0, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 
0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000, 0xffe0, 0xf7e0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000
};
//bocca semi-aperta
uint16_t pacman1 [] = {
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 
0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000, 0xffe0, 
0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0x0020, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 
0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0xf7e0, 0xffe0, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000
};
//bocca aperta
uint16_t pacman2 [] = {
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 
0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 
0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0x0000, 0x0020, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0x0000, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0xffe0, 0xffe0, 0xffe0, 0xffe0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000
};


void clearLives(int lives){
	int	x, y, cnt;
	for (cnt = 0; cnt<lives; cnt++){
		int i=0;
		for (y = 0; y < 15; y++) {
			for (x = 0; x < 15; x++) {
				LCD_SetPoint(x+5 + cnt*20, (y+300), empty_life[i]);
				i++;
			}
		}
	}
}


uint16_t *pacmans [] = {pacman0, pacman1, pacman2, pacman1};
int pacman_index = 1;

extern int nextIsWallDirection(int x0, int y0, int current_xspd, int current_yspd);
extern int isGoingToCollide();

int current_xspd, current_yspd;
int x0,y0;

int gameover = 0;
int view_blinky = 1;
int timer_spawn_blinky=-1;
int x1, y1;

current_xspd = -XSPD;
current_yspd = 0;
x0=50;
y0=100;

void checkAvailableInput(int x0, int y0, int direction) {
	switch (direction) {
		case 1: //down
			if (!nextIsWallDirection(x0, y0, 0, YSPD)) {
				current_xspd = 0;
				current_yspd = YSPD;
				nextInput = 0;
			}
		break;
		case 2: //left
			if (!nextIsWallDirection(x0, y0, -XSPD, 0)) {
				current_xspd = -XSPD;
				current_yspd = 0;
				nextInput = 0;
			}
			break;
		case 3: //up
			if (!nextIsWallDirection(x0, y0, 0, -YSPD)) {
				current_xspd = 0;
				current_yspd = -YSPD;
				nextInput = 0;
			}
			break;
		case 4: //right
			if (!nextIsWallDirection(x0, y0, XSPD, 0)) {
				current_xspd = XSPD;
				current_yspd = 0;
				nextInput = 0;
			}
			break;
		default: 
			break;
	}
}

extern int play_music;
extern int superPacman,superTimer;
extern void drawDoor(int x0, int y0);

void showGameOver(){

	gameover = 1;

	int x, y, i=0;
	int x0 = 80;
	int y0 = 10;

	disable_timer(0);
	disable_timer(1);
	disable_timer(2);
	disable_timer(3);

	LCD_Clear(Black);


	for(y=0; y<36; y++){
		for(x=0; x<80; x++){
			LCD_SetPoint(x+x0, y+y0, ending_image[i]);
			i++;
		}
	}

	GUI_Text(88, 200, (uint8_t*) "GAME OVER", Black, White);
	GUI_Text(40, 280, (uint8_t*) "Press RESET to restart", Black, White);


}

void showVictory(){

	gameover = 1;

	int x, y, i=0;
	int x0 = 80;
	int y0 = 10;

	disable_timer(0);
	disable_timer(1);
	disable_timer(2);
	disable_timer(3);

	LCD_Clear(Black);


	for(y=0; y<36; y++){
		for(x=0; x<80; x++){
			LCD_SetPoint(x+x0, y+y0, ending_image[i]);
			i++;
		}
	}

	GUI_Text(88, 200, (uint8_t*) "VICTORY!!", Black, White);
	GUI_Text(40, 280, (uint8_t*) "Press RESET to restart", Black, White);


}


void TIMER0_IRQHandler (void){
		
	if(LPC_TIM0->IR & 1) { // MR0 
		
		// play music at the beginning
		
		if(play_music){
			
		static int sineticks=0;
		/* DAC management */	
		static int currentValue; 
		currentValue = SinTable[sineticks];
		currentValue -= 410;
		currentValue /= 1;
		currentValue += 410;
		LPC_DAC->DACR = currentValue <<6;
		sineticks++;
		if(sineticks==45) sineticks=0;
			
		}else{
			
			if(timer%5 == 0 && timer<60){
				ghost_speed*=0.9;
				init_timer(2, 0, 0, 3, ghost_speed);
				reset_timer(2);
				enable_timer(2);
			}
		
			// UPDATE REMAINING TIME ON GUI
			if (!gameover && !pause){
				timer --;
				CAN_send();
			}

			if(timer <= superTimer && superTimer!=0){
				superPacman=0;
				superTimer=0;
			}
			
			if(timer==temp){
				decrease_lives = 1;
			}
			
			if(timer == timer_spawn_blinky){
				view_blinky = 1;
				x1 = 110;
				y1 = 150;
			}

			// GAME OVER
			if (timer == 0 || gameover){
				showGameOver();
			}
		}
		
		LPC_TIM0->IR = 1;			//clear interrupt flag

	}
	
	else if(LPC_TIM0->IR & 2){ // MR1
		// your code
		LPC_TIM0->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 4){ // MR2
		// your code
		LPC_TIM0->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 8){ // MR3
		// your code	
		LPC_TIM0->IR = 8;			// clear interrupt flag 
	}
  return;
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

int animation_cnt=0;
    
void TIMER1_IRQHandler (void) {
	
	if(LPC_TIM1->IR & 1){ // MR0

		// UPDATE PACMAN'S POSITION ON GUI
	if(!gameover){
			LCD_DisplayPacMan(pacmans[pacman_index], 10, 10, x0, y0, current_xspd, current_yspd);	
	}
		
		if (pause==0) {
			// collide = isGoingToCollide();
			if (nextInput != 0) {
				checkAvailableInput(x0, y0, nextInput);
			}
			if(!isGoingToCollide()){
				if (current_xspd != 0){
					x0+=current_xspd;
					if(x0>239){
						x0 = 0;
					}else if (x0<0){
						x0 = 239;
					}
				}else if (current_yspd != 0){
					y0+=current_yspd;
				}
				currentIsPill();
				if(animation_cnt==0){
					pacman_index = (pacman_index+1)%4;
				}
				animation_cnt = (animation_cnt+1)%4;
			}
		}
		
		LPC_TIM1->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM1->IR & 2){ // MR1
		// your code
		LPC_TIM1->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 4){ // MR2
		// your code	
		LPC_TIM1->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 8){ // MR3
		// your code	
		LPC_TIM1->IR = 8;			// clear interrupt flag 
	} 

	return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
uint16_t blinky1 [] = { 
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xf8c0, 0xf8c0, 0xf8c0, 
0xf8c0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0x0000, 0x0000, 0x0000, 0xf8c0, 
0xf8c0, 0xffff, 0xffff, 0xf8c0, 0xffff, 0xffff, 0xf8c0, 0x0000, 0x0000, 0xf8c0, 0xf8c0, 0xffff, 0x3a96, 0xf8c0, 0xffff, 0x3a96, 
0xf8c0, 0x0000, 0x0000, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0x0000, 0x0000, 0xf8c0, 0xf8c0, 0xf8c0, 
0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0x0000, 0x0000, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0x0000, 
0x0000, 0xf8c0, 0x0000, 0xf8c0, 0x0000, 0x0000, 0xf8c0, 0x0000, 0xf8c0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000
};

uint16_t blinky2 [] ={
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xf8c0, 0xf8c0, 0xf8c0, 
0xf8c0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0x0000, 0x0000, 0x0000, 0xf8c0, 
0xf8c0, 0xffff, 0xffff, 0xf8c0, 0xffff, 0xffff, 0xf8c0, 0x0000, 0x0000, 0xf8c0, 0xf8c0, 0xffff, 0x3a96, 0xf8c0, 0xffff, 0x3a96, 
0xf8c0, 0x0000, 0x0000, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0x0000, 0x0000, 0xf8c0, 0xf8c0, 0xf8c0, 
0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0x0000, 0x0000, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0xf8c0, 0x0000, 
0x0000, 0x0000, 0xf8c0, 0x0000, 0xf8c0, 0xf8c0, 0x0000, 0xf8c0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000
};

uint16_t scared_blinky1 [] ={
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003f, 0x003f, 0x003f, 
0x003f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x0000, 0x0000, 0x0000, 0x003f, 
0x003f, 0xffff, 0xffff, 0x003f, 0xffff, 0xffff, 0x003f, 0x0000, 0x0000, 0x003f, 0x003f, 0xffff, 0x3a96, 0x003f, 0xffff, 0x3a96, 
0x003f, 0x0000, 0x0000, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x0000, 0x0000, 0x003f, 0x003f, 0x003f, 
0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x0000, 0x0000, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x0000, 
0x0000, 0x0000, 0x003f, 0x0000, 0x003f, 0x003f, 0x0000, 0x003f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000
};

uint16_t scared_blinky2 []={
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003f, 0x003f, 0x003f, 
0x003f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x0000, 0x0000, 0x0000, 0x003f, 
0x003f, 0xffff, 0xffff, 0x003f, 0xffff, 0xffff, 0x003f, 0x0000, 0x0000, 0x003f, 0x003f, 0xffff, 0x3a96, 0x003f, 0xffff, 0x3a96, 
0x003f, 0x0000, 0x0000, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x0000, 0x0000, 0x003f, 0x003f, 0x003f, 
0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x0000, 0x0000, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x003f, 0x0000, 
0x0000, 0x003f, 0x0000, 0x003f, 0x0000, 0x0000, 0x003f, 0x0000, 0x003f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
0x0000, 0x0000, 0x0000, 0x0000
};

uint16_t *blinkys [] = {blinky1, blinky2};
uint16_t *scared_blinkys [] = {scared_blinky1, scared_blinky2};
int blinky_index = 0;
x1 = 110;
y1 = 150;
int ghost_xspd, ghost_yspd;
ghost_xspd=0;
ghost_yspd=1;

extern int score,lives;
int b_cnt=0;
 
extern void addToRestoreBuffer(int tile_x, int tile_y);
extern void restorePills();
extern void displayLives(int lives);
extern void restoreEatenBlinkySpot(int x_tile, int y_tile);

void TIMER2_IRQHandler (void) {
	if(LPC_TIM2->IR & 1) // MR0
		{
		int current_x = (x1)/10;
		int current_y = (y1-40)/10;
		int pacmanx_tile = x0/10;
		int pacmany_tile = (y0-40)/10;

		if (x1%10==0 && y1%10==0) {		// quando Blinky è completamente incluso in un tile tengo traccia della pillola mangiata 
			
			// aggiunta al buffer di ricostruzione solo se Blinky è in movimento
			if (ghost_xspd != 0 || ghost_yspd != 0) {
				addToRestoreBuffer(current_x, current_y);
			}
			
			if (current_x >= 10 && current_x <= 13 && current_y >=9 && current_y <= 12) { // se Blinky è nella scatola centrale allora esce, a prescindere dalla modalità
				ghost_xspd = 0;
				ghost_yspd = -1;
			} else {
			// Check if at a junction
			int distances[4] = {9999, 9999, 9999, 9999}; // distances in order: right, left, down, up
				// Calculate distances to Pacman
				if (ghost_xspd != -1 && !is_wall(current_x + 1, current_y)) distances[0] = abs((current_x + 1) - pacmanx_tile) + abs(current_y - pacmany_tile);
				if (ghost_xspd != 1 && !is_wall(current_x - 1, current_y)) distances[1] = abs((current_x - 1) - pacmanx_tile) + abs(current_y - pacmany_tile);
				if (ghost_yspd != -1 && !is_wall(current_x, current_y + 1)) distances[2] = abs(current_x - pacmanx_tile) + abs((current_y + 1) - pacmany_tile);
				if (ghost_yspd != 1 && !is_wall(current_x, current_y - 1)) distances[3] = abs(current_x - pacmanx_tile) + abs((current_y - 1) - pacmany_tile);

				// Find the direction with the minimum distance
				int max_distance = -1;
				int min_distance = 9999;
				int direction = -1;
				int i;

				if(superPacman){
					for (i = 0; i < 4; i++) {
						if (distances[i] > max_distance && distances[i]<9999) {
							max_distance = distances[i];
							direction = i;
						}
					}
				}else{
					for (i = 0; i < 4; i++) {
						if (distances[i] < min_distance) {
							min_distance = distances[i];
							direction = i;
						}
					}
				}

				// Set ghost direction based on minimum distance
				switch (direction) {
					case 0: ghost_xspd = 1; ghost_yspd = 0; break; // right
					case 1: ghost_xspd = -1; ghost_yspd = 0; break; // left
					case 2: ghost_xspd = 0; ghost_yspd = 1; break; // down
					case 3: ghost_xspd = 0; ghost_yspd = -1; break; // up
				}
			}
		}else{			// quando blinky NON è più completamente incluso in un tile ricostruisco la pillola mangiata 
			

			if(((x1-7)%10==0 || (y1-7)%10==0) && (ghost_xspd>=0 && ghost_yspd>=0)){				//moving down or right	
				restorePills();
			}else if(((x1-2)%10==0 || (y1-2)%10==0) && (ghost_xspd<=0 && ghost_yspd<=0)){ 		//moving up or left	
				restorePills();
			}

		}

		x1 += ghost_xspd;
		if(x1<0) x1=239;
		if(x1>239) x1=0;
		y1 += ghost_yspd;

		if(view_blinky==1){
			if(superPacman){
				LCD_DisplayBlinky(scared_blinkys[blinky_index], 10, 10, x1, y1, ghost_xspd, ghost_yspd);
			
			}else{
				LCD_DisplayBlinky(blinkys[blinky_index], 10, 10, x1, y1, ghost_xspd, ghost_yspd);
			}
		}
		
		if(current_y==8 && (current_x==10 || current_x==13)){
			drawDoor(110, 130);
		}
		
		
		if(current_x == pacmanx_tile && current_y == pacmany_tile){
			temp=timer-2;
			if(!superPacman){
				lives--;
				if (lives <= 0) {
					gameover = 1;
				} else {
					x0 = 50;
					y0 = 100;
					clearLives(lives+1);
				}
			}else{
				int x_to_restore = current_x;
				int y_to_restore = current_y;

				score+=100;
				view_blinky = 0;				
				x1 = 0;
				y1 = 300;
				ghost_xspd=0;
				ghost_yspd=0;
			  timer_spawn_blinky = timer-3;
				
				restoreEatenBlinkySpot(x_to_restore, y_to_restore);			// questa funzione è utile per ricostruire la mappa nel punto in cui blinky viene mangiato

				
			}
			CAN_send();
		}
	
		if(b_cnt%4==0){
			blinky_index = (blinky_index +1) %2;
		}
		b_cnt = (b_cnt +1) %4;
		
		LPC_TIM2->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM2->IR & 2){ // MR1
		// your code
		LPC_TIM2->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM2->IR & 4){ // MR2
		// your code	
		LPC_TIM2->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM2->IR & 8){ // MR3
		// your code	
		LPC_TIM2->IR = 8;			// clear interrupt flag 
	} 
  return;
}


/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER3_IRQHandler (void) {
	if(LPC_TIM3->IR & 1) // MR0
	{ 
		disable_timer(0);
		LPC_TIM3->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM3->IR & 2){ // MR1
		// your code
		LPC_TIM3->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 4){ // MR2
		// your code	
		LPC_TIM3->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 8){ // MR3
		// your code	
		LPC_TIM3->IR = 8;			// clear interrupt flag 
	} 

  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
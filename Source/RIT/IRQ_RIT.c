/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../shared.h"
#include "../led/led.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"
#include "../adc/adc.h"
#include "../music/music.h"

#define UPTICKS 1


/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;

extern int play_music;


NOTE pacman_theme[] = 
{
	//1
	{b2, time_croma},
	{b3, time_croma},
	{f3d, time_croma},
	{e3b, time_croma},
	//2
	{b3, time_croma},
	{f3, time_croma},
	{e3, time_semiminima},
	
	//3
	{c3, time_croma},
	{c4, time_croma},
	{g3, time_croma},
	{e3, time_croma},
	//4
	{c4, time_croma},
	{f3, time_croma},
	{e3, time_semiminima},
	
	//5
	{b2, time_croma},
	{b3, time_croma},
	{f3d, time_croma},
	{e3b, time_croma},
	//6
	{b3, time_croma},
	{f3, time_croma},
	{e3, time_semiminima},
	
	{silence, time_semibiscroma},
	
	//7
	{f3, time_croma},
	{g3, time_croma},
	//8
	{a3, time_croma},
	{b3, time_croma},
	{e4, time_croma},
};


int curnote = 0;
int play_music = 1;


extern BOOL isNotePlaying();
extern void playNote(NOTE note);


void playPacmanTheme(){
	static int currentNote = 0;
	static int ticks = 0;
	
	if(!isNotePlaying()){
		++ticks;
		if(ticks == UPTICKS)
		{
			ticks = 0;
			playNote(pacman_theme[currentNote++]);
		}
	}
	
	if(currentNote == (sizeof(pacman_theme) / sizeof(pacman_theme[0]))){
		// currentNote=0;
		play_music=0;
		disable_timer(0);
		init_timer(0, 0, 0, 3, 0x17D7840);	//25.000.000 == 1
		enable_timer(0);
		//disable_RIT();
	}

	curnote = currentNote;
//   LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
}


extern int current_xspd, current_yspd;
extern int pause, gameover;
extern int nextInput;

void RIT_IRQHandler (void){		
	
	static int j_select=0, j_left=0, j_right=0, j_down=0, j_up=0;
	
	ADC_start_conversion();
	
	/*******************SOUNDS*********************/

	if(play_music){
		playPacmanTheme();
	}

	/*******************POLLING JOYSTICK*********************/

	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){ //joystick select
		j_select++;
		switch(j_select){
			case 1:
				break;
			//per controllare pressioni prolungate: es. 5[sec]/50[msec](RIT timer) = 100
			//case 100:
			//	break;
			default:
				break;
		}
	}else
		j_select=0;
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){ //joystick down
		j_down++;
		switch(j_down){
			case 1:
				nextInput = 1;
				break;			
			default:
				break;
		}
	}else
		j_down=0;
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){ //joystick left
		j_left++;
		switch(j_left){
			case 1:
				nextInput = 2;
				break;			
			default:
				break;
		}
	}else
		j_left=0;
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){ //joystick right
		j_right++;
		switch(j_right){
			case 1:
				nextInput = 4;
				break;			
			default:
				break;
		}
	}else
		j_right=0;
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){ //joystick up
		j_up++;
		switch(j_up){
			case 1:
				nextInput = 3;
				break;			
			default:
				break;
		}
	}else
		j_up=0;
//}

/*************************INT0***************************/
if(down_0 !=0){
	down_0++;
	if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
		switch(down_0){
			case 2:
			if(!gameover && !play_music){
				if (pause == 0) {
					GUI_Text(x_pause, y_pause, (uint8_t *) "PAUSE", Black, White);
					disable_timer(0);
					disable_timer(1);
					disable_timer(2);
					pause = 1;
				} else if (pause==1) {
					GUI_Text(x_pause, y_pause, (uint8_t *) "     ", Black, Black);
					enable_timer(0);
					enable_timer(1);
					enable_timer(2);
					pause = 0;
				}
				break;
			}
				
			//per controllare pressioni prolungate: es. 5[sec]/50[msec](RIT timer) = 100
				//case 100:
				//	break;
			default:
				//se voglio gestire il button on X click
					//if(down_0 % X == 0){
					//}		
				break;
		}
	}
	else {	/* button released */
		//al rilascio del bottone il valore di down*valore inizializzazione RIT (50ms di solito) indica per quanti ms � stato premuto il pulsante
		down_0=0;			
		NVIC_EnableIRQ(EINT0_IRQn);							 /* disable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
	}
} // end INT0

/*************************KEY1***************************/
if(down_1 !=0){
	down_1++;
	if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
		switch(down_1){
			case 2:
				// your code
				break;
			default:
				break;
		}
	}
	else {	/* button released */
		down_1=0;			
		NVIC_EnableIRQ(EINT1_IRQn);							 /* disable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
	}
} // end KEY1

/*************************KEY2***************************/
if(down_2 !=0){
	down_2++;
	if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
		switch(down_2){
			case 2:
				// your code
				break;
			default:
				break;
		}
	}
	else {	/* button released */
		down_2=0;		
		NVIC_EnableIRQ(EINT2_IRQn);							 /* disable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
	}
} // end KEY2
	
	reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

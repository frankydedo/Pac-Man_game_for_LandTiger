#include "music.h"
#include "LPC17xx.h"
#include "../timer/timer.h"
#include "GLCD/GLCD.h" 

void playNote(NOTE note){

	int tim2_cnt = 0;

	if(note.freq != silence){

		reset_timer(0);
		init_timer(0, 0, 0, 3, 0.3*note.freq);						
		enable_timer(0);

		tim2_cnt++;
	}

	reset_timer(3);
	init_timer(3, 0, 0, 7, 0.55*note.duration);					
	enable_timer(3);
}

BOOL isNotePlaying(){

	return ((LPC_TIM0->TCR != 0) || (LPC_TIM3->TCR != 0));;
}

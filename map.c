#define map_width 24
#define map_height 24

#include "GLCD/GLCD.h"

extern void showVictory();

typedef struct{
	int is_wall;
	int is_pill;
	int is_powerpill;
}Tile;

typedef struct{
	int cord_x;
	int cord_y;
	int isPowerPill;
}PillToRestore;

extern int current_xspd, current_yspd;
extern int x0, y0;
extern int gameover;

Tile map[map_width][map_height];
int cord_x, cord_y;
int random_buffer[240]; 
int score = 0;
char stringscore[10];
int cord_x, cord_y;
int remainingPills = 0;

#include "../CAN/CAN.h"                      /* LPC17xx CAN adaption layer */
extern int lives, timer;

void CAN_send(void) {
	
	CAN_TxMsg.data[0] = (lives);
	CAN_TxMsg.data[1] = (timer);
	CAN_TxMsg.data[2] = (score & 0xFF00 ) >> 8;
	CAN_TxMsg.data[3] = score & 0xFF;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 1;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg (2, &CAN_TxMsg);           
	
	/*	for debbugging purposes:
	
	char remainingTime[3];
char stringscore[5];

	sprintf(remainingTime, "%02d", timer);
		sprintf(stringscore, "%d", score);
	
		GUI_Text(105, 0, (uint8_t *) remainingTime, White, Black);
		GUI_Text(48, 17, (uint8_t *) stringscore, Black, White);
		if (!gameover) displayLives(lives);
		*/
}


extern unsigned short AD_current;

void initializeBuffer(){
	int i;
	for (i = 0; i < 240; i++) {
		random_buffer[i] = 0;
	}
	int count = 0;
	while (count < 6) {
	srand(AD_current);
		int index = rand() % 240;
		if (random_buffer[index] == 0) {
				random_buffer[index] = 1;
				count++;
		}
	}
}

Tile createTile(int is_wall, int is_pill, int is_powerpill) {
    Tile tile;
    tile.is_wall = is_wall;
    tile.is_pill = is_pill;
    tile.is_powerpill = is_powerpill;
    return tile;
}

PillToRestore createPillToRestore(int cord_x, int cord_y, int isPowerPill){
	PillToRestore pill;
	pill.cord_x=cord_x;
	pill.cord_y=cord_y;
	pill.isPowerPill=isPowerPill;

	return pill;
}


// 0 = spazio vuoto
// 1 = muro
// 2 = pillola
int layout[map_width][map_height] = {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 ,1},
		{1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1},
		{1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1},
		{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 ,1},
		{1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2 ,1},
		{1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1 ,2, 2, 2, 2, 2, 1},
		{1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1 ,1, 1, 2, 1, 1, 1, 1, 1},
		{0, 0, 0, 0, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 0, 0, 0, 0},
		{1, 1, 1 ,1, 1, 2, 1, 2, 1, 1, 1, 3, 3, 1 ,1, 1, 2, 1, 2, 1, 1, 1, 1, 1},
		{0, 0, 2, 2, 2, 2, 1, 2, 1, 1, 0, 0, 0, 0, 1, 1, 2, 1, 2, 2, 2, 2, 0, 0},
		{0, 0, 1, 1, 1, 2, 2, 2, 1, 1, 0, 0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 1, 0, 0},
		{0, 0, 2, 2, 2, 2, 1, 2, 1, 1, 0, 0, 0, 0, 1, 1, 2, 1, 2, 2, 2, 2, 0, 0},
		{1, 1, 1, 1, 1, 2, 1, 2, 1, 1 ,1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1},
		{0, 0, 0, 0, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1 ,1, 1, 1, 1},
		{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
		{1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1},
		{1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 1},
		{1, 1, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 1, 1},
		{1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1},
		{1, 2, 1, 1, 1, 1 ,1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
		{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 ,1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};


void showScore(char *score){
	GUI_Text(140, 15, (uint8_t *) score, White, Black);
}

void victory() {
	gameover=1;

	LCD_Clear(Black);
	GUI_Text(10, 10, (uint8_t*) "YOU WIN", Black, White);
	disable_timer(0);
	disable_timer(1);
}

void initializeMap(){
	int i, j, buff_index=0;
	int powerpill_num = 6;
	for (i = 0; i < map_height; i++) {
        for (j = 0; j < map_width; j++) {
            if ((layout[i][j] == 1) || (layout[i][j] == 3)) {
                map[i][j] = createTile(1, 0, 0); // Muro
            } else if (layout[i][j] == 2) {
								if(random_buffer[buff_index]==0){
									map[i][j] = createTile(0, 1, 0); // Pillola
								}else{
									map[i][j] = createTile(0, 0, 1); // Super Pillola
								}
								remainingPills++;
								buff_index++;
            } else {
                map[i][j] = createTile(0, 0, 0); // Spazio vuoto
            }
        }
    }
}

	
void drawWall(int x0, y0){
	int i,j;
	for(i=0; i<10;i++){
		for(j=0; j<10; j++){
			LCD_SetPoint((x0+j), (y0+i), Blue);
		}
	}
}

void drawPill(int x0, y0){
	int i, j;
	for(i=0; i<2;i++){
		for(j=0;j<2;j++){
			LCD_SetPoint((x0+j+4), (y0+i+4), Cyan);
		}
	}
}

void drawSuperPill(int x0, y0){
	int i,j;

	for(i=0; i<10;i++){
		for(j=0; j<10; j++){
			if(j==2 || j==7){
				if (i==4 || i==5){
					LCD_SetPoint((x0+j), (y0+i), Cyan);
				}
			}else if(j==3 || j==6){
				if (i<=6 && i>=3){
					LCD_SetPoint((x0+j), (y0+i), Cyan);
				}
			}else if(j==4 || j==5){
				if (i<= 7 && i>=2){
					LCD_SetPoint((x0+j), (y0+i), Cyan);
				}
			}
		}
	}
}


/*
void drawSuperPill(int x0, y0){
	int i,j;
	for (i=1; i<8;i++){
		for(j=1;j<8;j++){
			if ((i!=1 && j!=1)&&(i!=1 && j!=8)&&(i!=8 && j!=1)&&(i!=8 && j!=8)){
					LCD_SetPoint((x0+j), (y0+i), White);	
			}
		}
	}
}
*/

void drawDoor(int x0, y0){
	int i,j;
	for(i=0; i<10;i++){
		for(j=0; j<10; j++){
			if(i==0){
				LCD_SetPoint((x0+j), (y0+i), Green);
			}else{
				LCD_SetPoint((x0+j), (y0+i), Black);
			}
		}
	}
}


void drawMap (){
	initializeBuffer();
	initializeMap();
	int i,j;
	int y0 = 40;
	for(i=0;i<map_height;i++){
		for (j=0;j<map_width;j++){
			if (map[i][j].is_pill){
				drawPill(j*10,y0+i*10);
			}
			else if (map[i][j].is_powerpill){
				drawSuperPill(j*10,y0+i*10);
			}
			else if(map[i][j].is_wall && layout[i][j] != 3){
				drawWall(j*10,y0+i*10);
			}else if (layout[i][j] == 3){
				drawDoor(j*10,y0+i*10);
			}
		
		}
	}

}


extern uint16_t pacman1[];
extern int lives, scoreLivesCounter,timer;
int superPacman = 0;
int superTimer = 0;

void currentIsPill(){
	//qui bisogna controllare il tile in cui si trova pacman
	//occhio alle coordinate
	if(map[cord_y][cord_x].is_pill){
		map[cord_y][cord_x].is_pill = 0;
		score += 10;
		scoreLivesCounter += 10;
		CAN_send();
		remainingPills--;
	}
	else if(map[cord_y][cord_x].is_powerpill){
		map[cord_y][cord_x].is_powerpill = 0;
		superPacman = 1;
		superTimer = timer-10;
		scoreLivesCounter += 50;
		score += 50;
		CAN_send();
		remainingPills--;
	}
	if (scoreLivesCounter >= 1000) {
		lives ++;
		scoreLivesCounter -= 1000;
		CAN_send();
	}
	if (remainingPills <= 0) {
		showVictory();
	}
}

int isGoingToCollide(){
	currentIsPill();
	if(current_xspd != 0){ 			// moving horizontally
		if(x0%10==0 && nextIsWall()) return 1;
	}else if(current_yspd != 0){ 	// moving vertically
		if(y0%10==0 && nextIsWall()) return 1;
	}

	return 0;
}

int nextIsWall(){
	// pixels coordinates
	int x=x0;
	int y=y0-40; 

	// tiles cordinates
	cord_x = (x/10);
	cord_y = (y/10);

	Tile nextTile;

	if(current_xspd > 0){       // moving right
		nextTile = map[cord_y][cord_x+1 > 23 ? 0 : cord_x+1];
	}else if(current_xspd < 0){ // moving left
		nextTile = map[cord_y][cord_x-1 < 0 ? 23 : cord_x-1];
	}else if(current_yspd > 0){ // moving down
		nextTile = map[cord_y+1][cord_x];
	}else if(current_yspd < 0){ // moving up
		nextTile = map[cord_y-1][cord_x];
	}

	if(nextTile.is_wall){
		return 1;
	}else return 0;
}

int nextIsWallDirection(int x0, int y0, int current_xspd, int current_yspd) {
	// pixels coordinates
	int x=x0;
	int y=y0-40; 

	// tiles cordinates
	cord_x = (x/10);
	cord_y = (y/10);

	if(x0%10 != 0 || y0%10 != 0){ 			// moving vertically
		return 1;
	}

	Tile nextTile;
	if(current_xspd > 0){       // moving right
		nextTile = map[cord_y][cord_x+1 > 23 ? 0 : cord_x+1];
	}else if(current_xspd < 0){ // moving left
		nextTile = map[cord_y][cord_x-1 < 0 ? 23 : cord_x-1];
	}else if(current_yspd > 0){ // moving down
		nextTile = map[cord_y+1][cord_x];
	}else if(current_yspd < 0){ // moving up
		nextTile = map[cord_y-1][cord_x];
	}

	if(nextTile.is_wall){
		return 1;
	}else return 0;
}

int is_wall(int tile_x, int tile_y) {

	// Return whether the tile at the given coordinates is a wall
	return map[tile_y][tile_x].is_wall;
}



// buffer di ricostruzione delle pillole


PillToRestore pills_to_restore[];
int buffer_index=0;	//punta al primo spazio libero

void addToRestoreBuffer(int tile_x, int tile_y){

	if(map[tile_y][tile_x].is_powerpill){
		PillToRestore ptr = createPillToRestore(tile_x*10, (tile_y*10)+40, 1);
		pills_to_restore[buffer_index] = ptr;
		buffer_index++;
		return;
	}else if(map[tile_y][tile_x].is_pill){
		PillToRestore ptr = createPillToRestore(tile_x*10, (tile_y*10)+40, 0);
		pills_to_restore[buffer_index] = ptr;
		buffer_index++;
		return;
	}
	
}

void restorePills(){

	if(buffer_index>0){
		
		PillToRestore ptr = pills_to_restore[buffer_index-1];

		if(ptr.isPowerPill){
			drawSuperPill(ptr.cord_x, ptr.cord_y);
			buffer_index--;
		}else{
			drawPill(ptr.cord_x, ptr.cord_y);
			buffer_index--;
		}
		
	}

}

void drawBlack10x10(int x0, int y0){
	int i,j;
	
	for(i=x0; i<x0+10; i++){
		for(j=y0; j<y0+10; j++){
			LCD_SetPoint(i, j, Black);
		}
	}
}

void restoreEatenBlinkySpot(int x_tile, int y_tile){
	
	int i, j;
	
	for(i=x_tile-1; i<x_tile+2; i++){
		for(j=y_tile-1; j<y_tile+2; j++){
			
			if(map[j][i].is_pill){
				drawPill(i*10, (j*10)+40);
			}else if(map[j][i].is_powerpill){
				drawSuperPill(i*10, (j*10)+40);
			}else if(map[j][i].is_wall){
				drawWall(i*10, (j*10)+40);
			}else{
				drawBlack10x10(i*10, (j*10)+40);
			}
		}
	}

}



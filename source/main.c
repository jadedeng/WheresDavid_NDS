/*
 * Template Nintendo DS
 * May 2011
 */

#include <nds.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "beach.h"
#include "medieval.h"
#include "park.h"
#include "score.h"
#include "splash.h"
#include "menu.h"
#include "final.h"
#include "funny.h"
#include "underwater.h"
#include <maxmod9.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#define SEARCH_WIDTH 40
#define SEARCH_HEIGHT 40
#define TOP_SEARCH_X 0
#define TOP_SEARCH_Y 0
#define MAX_TIME 150
#define NUM_MAPS 5
#define CURSOR_SIZE 16
#define CURSOR_MOVE_FACTOR 2

void initBottomGamescreen();
void displayNextBottomScreen();
void initTopGamescreen();
void displayTopSearchFor(int x, int y);
void displayTopTimeBar(int timeLeft);
void initTimer();
void timerISR();
void initSound();
void playNextMusic();
void updateScore();
void initScore();
void changeCursor(int down, int right);
void initSplashScreen();
void initMainMenu();
void initFinalScreen();

int timer_ticks;
int score;
int searchForX;
int searchForY;
int bottomMaps[NUM_MAPS];
int bottomMapLens[NUM_MAPS];
int bottomPals[NUM_MAPS];
int bottomPalLens[NUM_MAPS];
int mapMusic[NUM_MAPS];
int currentBottomMap;
int cursorX;
int cursorY;
int state; //0 is menu, 1 is playing game, 2 is finish screen
bool sound;

u8 emptyTile[64] =
{
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
};

u8 digit0[64] =
{
	0,0,0,2,2,0,0,0,
	0,0,2,2,2,2,0,0,
	0,2,2,0,0,2,2,0,
	0,2,2,0,0,2,2,0,
	0,2,2,0,0,2,2,0,
	0,2,2,0,0,2,2,0,
	0,0,2,2,2,2,0,0,
	0,0,0,2,2,0,0,0
};

u8 digit1[64] =
{
	0,0,0,2,2,0,0,0,
	0,0,2,2,2,0,0,0,
	0,0,0,2,2,0,0,0,
	0,0,0,2,2,0,0,0,
	0,0,0,2,2,0,0,0,
	0,0,0,2,2,0,0,0,
	0,0,0,2,2,0,0,0,
	0,0,2,2,2,2,0,0
};

u8 digit2[64] =
{
	0,0,0,2,2,0,0,0,
	0,0,2,2,2,2,0,0,
	0,2,2,0,0,2,2,0,
	0,0,0,0,0,2,2,0,
	0,0,0,0,2,2,0,0,
	0,0,0,2,2,0,0,0,
	0,0,2,2,2,2,2,0,
	0,2,2,2,2,2,2,0
};

u8 digit3[64] =
{
	0,0,0,2,2,0,0,0,
	0,0,2,2,2,2,0,0,
	0,2,2,0,0,2,2,0,
	0,0,0,0,2,2,0,0,
	0,0,0,0,2,2,0,0,
	0,2,2,0,0,2,2,0,
	0,0,2,2,2,2,0,0,
	0,0,0,2,2,0,0,0
};

u8 digit4[64] =
{
	0,0,0,2,2,2,0,0,
	0,0,2,2,2,2,0,0,
	0,2,2,0,2,2,0,0,
	2,2,0,0,2,2,0,0,
	2,2,2,2,2,2,2,0,
	0,0,0,0,2,2,0,0,
	0,0,0,0,2,2,0,0,
	0,0,0,2,2,2,2,0
};

u8 digit5[64] =
{
	0,2,2,2,2,2,2,0,
	0,2,2,2,2,2,2,0,
	0,2,2,0,0,0,0,0,
	0,2,2,2,0,0,0,0,
	0,0,2,2,2,2,0,0,
	0,0,0,0,0,2,2,0,
	0,2,2,0,0,2,2,0,
	0,0,2,2,2,2,0,0
};

u8 digit6[64] =
{
	0,0,0,0,2,2,0,0,
	0,0,0,2,2,0,0,0,
	0,0,2,2,0,0,0,0,
	0,2,2,0,0,0,0,0,
	0,2,2,2,2,2,0,0,
	0,2,2,0,0,2,2,0,
	0,2,2,0,0,2,2,0,
	0,0,2,2,2,2,0,0,
	0,0,0,2,2,0,0,0
};

u8 digit7[64] =
{
	2,2,2,2,2,2,2,0,
	2,2,2,2,2,2,2,0,
	0,0,0,0,0,2,2,0,
	0,0,0,0,0,2,2,0,
	0,0,0,0,2,2,0,0,
	0,0,0,2,2,0,0,0,
	0,0,2,2,0,0,0,0,
	0,2,2,0,0,0,0,0
};

u8 digit8[64] =
{
	0,0,2,2,2,2,0,0,
	0,2,2,0,0,2,2,0,
	0,2,2,0,0,2,2,0,
	0,0,2,2,2,2,0,0,
	0,0,2,2,2,2,0,0,
	0,2,2,0,0,2,2,0,
	0,2,2,0,0,2,2,0,
	0,0,2,2,2,2,0,0
};

u8 digit9[64] =
{
	0,0,0,2,2,0,0,0,
	0,0,2,2,2,2,0,0,
	0,2,2,0,0,2,2,0,
	0,2,2,0,0,2,2,0,
	0,0,2,2,2,2,2,0,
	0,0,0,0,0,2,2,0,
	0,0,0,0,2,2,0,0,
	0,0,0,2,2,0,0,0
};

u8 x1[64] = {
	1,1,0,0,0,0,0,0,
	1,1,1,0,0,0,0,0,
	0,1,1,1,0,0,0,0,
	0,0,1,1,1,0,0,0,
	0,0,0,1,1,1,0,0,
	0,0,0,0,1,1,1,0,
	0,0,0,0,0,1,1,1,
	0,0,0,0,0,0,1,1
};

u8 x0[64] = {
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
};

u8 x2[64] = {
	0,0,0,0,0,0,1,1,
	0,0,0,0,0,1,1,1,
	0,0,0,0,1,1,1,0,
	0,0,0,1,1,1,0,0,
	0,0,1,1,1,0,0,0,
	0,1,1,1,0,0,0,0,
	1,1,1,0,0,0,0,0,
	1,1,0,0,0,0,0,0
};

int main(void) {

    consoleDemoInit();
    state = 0;
    sound = true;

    while(1) {
    	if(state == 0) {
    		//Menu State
    		initSplashScreen();
    		initSound();
    		initMainMenu();
    	} else if(state == 1) {
    		//Play state
    		initBottomGamescreen();
			displayNextBottomScreen();
			if(sound) {
				playNextMusic();
			}
			initTopGamescreen();
			srand(time(0));
			searchForX = rand()%(256 - SEARCH_WIDTH);
			searchForY = rand()%(192 - SEARCH_HEIGHT);
			displayTopSearchFor(searchForX, searchForY);
			initTimer();
			while(state == 1) {
				// Scan the keys that have been pressed down
				scanKeys();
				unsigned downKeys = keysDown();
				unsigned keys = keysHeld();

				if(downKeys & KEY_TOUCH) {
					touchPosition touch;
					touchRead(&touch);
					if(touch.px >= searchForX && touch.px <= searchForX + SEARCH_WIDTH && touch.py >= searchForY && touch.py <= searchForY + SEARCH_HEIGHT) {
						//Wally Found!
						if(sound) {
							mmEffect(SFX_BOING);
						}
						updateScore();
						searchForX = rand()%(256 - SEARCH_WIDTH);
						searchForY = rand()%(192 - SEARCH_HEIGHT);
						displayTopSearchFor(searchForX, searchForY);
					}
				} else if(keys & KEY_LEFT) {
					changeCursor(0, -1);
				} else if(keys & KEY_RIGHT) {
					changeCursor(0, 1);
				} else if(keys & KEY_UP) {
					changeCursor(-1, 0);
				} else if(keys & KEY_DOWN) {
					changeCursor(1, 0);
				} else if(downKeys & KEY_A) {
					if(cursorX >= searchForX && cursorX <= searchForX + SEARCH_WIDTH && cursorY >= searchForY && cursorY <= searchForY + SEARCH_HEIGHT) {
						//Wally Found!
						if(sound) {
							mmEffect(SFX_BOING);
						}
						updateScore();
						searchForX = rand()%(256 - SEARCH_WIDTH);
						searchForY = rand()%(192 - SEARCH_HEIGHT);
						displayTopSearchFor(searchForX, searchForY);
					}
				}

				swiWaitForVBlank();
			}
    	} else if(state == 2) {
    		initSplashScreen();
    		initFinalScreen();
    	}
    	swiWaitForVBlank();
    }
}

void initBottomGamescreen() {

	VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;
	REG_DISPCNT_SUB = MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_BG2_ACTIVE;
	BGCTRL_SUB[3] = BG_BMP_BASE(0) | BgSize_B8_256x256;
	BGCTRL_SUB[2] = BG_BMP_BASE(3) | BgSize_B8_256x256;

	int i, j;
	uint8* myNewMem = (uint8*) BG_BMP_RAM_SUB(3);
	uint8 trans[2] = {0, 0};
	for(i=0; i<256; i++) {
		for(j=0; j<192; j++) {
			swiCopy(&trans, myNewMem + (i+256*j), 1);
		}
	}

	cursorX = 256/2;
	cursorY = 192/2;

	changeCursor(0, 0);

	REG_BG3PA_SUB = 256;
	REG_BG3PC_SUB = 0;
	REG_BG3PB_SUB = 0;
	REG_BG3PD_SUB = 256;

	REG_BG2PA_SUB = 256;
	REG_BG2PC_SUB = 0;
	REG_BG2PB_SUB = 0;
	REG_BG2PD_SUB = 256;

	currentBottomMap = -1;

	bottomMaps[0] = parkBitmap;
	bottomMapLens[0] = parkBitmapLen;
	bottomPals[0] = parkPal;
	bottomPalLens[0] = parkPalLen;

	bottomMaps[1] = beachBitmap;
	bottomMapLens[1] = beachBitmapLen;
	bottomPals[1] = beachPal;
	bottomPalLens[1] = beachPalLen;

	bottomMaps[2] = medievalBitmap;
	bottomMapLens[2] = medievalBitmapLen;
	bottomPals[2] = medievalPal;
	bottomPalLens[2] = medievalPalLen;

	bottomMaps[3] = underwaterBitmap;
	bottomMapLens[3] = underwaterBitmapLen;
	bottomPals[3] = underwaterPal;
	bottomPalLens[3] = underwaterPalLen;

	bottomMaps[4] = funnyBitmap;
	bottomMapLens[4] = funnyBitmapLen;
	bottomPals[4] = funnyPal;
	bottomPalLens[4] = funnyPalLen;

}


void displayNextBottomScreen() {
	currentBottomMap = currentBottomMap + 1;
	if(currentBottomMap < NUM_MAPS) {
		swiCopy(bottomPals[currentBottomMap], BG_PALETTE_SUB, bottomPalLens[currentBottomMap]/2);
		swiCopy(bottomMaps[currentBottomMap], BG_GFX_SUB, bottomMapLens[currentBottomMap]/2);
		swiCopy(bottomPals[currentBottomMap], BG_PALETTE, bottomPalLens[currentBottomMap]/2);
	} else {
		state = 2;
		irqDisable(IRQ_TIMER0);
	}
}


void initTopGamescreen() {
	VRAM_A_CR = VRAM_ENABLE | VRAM_A_MAIN_BG;
	REG_DISPCNT = MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG0_ACTIVE | DISPLAY_BG3_ACTIVE;

	BGCTRL[3] = BG_BMP_BASE(5) | BgSize_B8_256x256;

	REG_BG3PA = 256;
	REG_BG3PC = 0;
	REG_BG3PB = 0;
	REG_BG3PD = 256;

	swiCopy(scoreBitmap, BG_BMP_RAM(5), scoreBitmapLen/2);

	//This takes up the first 48KB in VRAM_A
	BGCTRL[2] = BG_BMP_BASE(0) | BgSize_B8_256x256;

	REG_BG2PA = 256;
	REG_BG2PC = 0;
	REG_BG2PB = 0;
	REG_BG2PD = 256;

	int i, j;
	uint8* myNewMem = (uint8*) BG_GFX;
	uint8 trans[2] = {0, 0};

	for(i=0; i < 256; i++) {
		for(j=0; j<192; j++) {
			swiCopy(&trans, myNewMem + (i+256*j), 1);
		}
	}

	//start at index tile base 3
	//Map base is 2KB therefore at 24 we have the start o BG_TILE_BASE which will take up
	//640 bytes i.e. only 1 map base.
	BGCTRL[0] = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(25) | BG_TILE_BASE(3);

	dmaCopy(emptyTile, &BG_TILE_RAM(3)[0], 64);
	dmaCopy(digit0, &BG_TILE_RAM(3)[32], 64);
	dmaCopy(digit1, &BG_TILE_RAM(3)[64], 64);
	dmaCopy(digit2, &BG_TILE_RAM(3)[96], 64);
	dmaCopy(digit3, &BG_TILE_RAM(3)[128], 64);
	dmaCopy(digit4, &BG_TILE_RAM(3)[160], 64);
	dmaCopy(digit5, &BG_TILE_RAM(3)[192], 64);
	dmaCopy(digit6, &BG_TILE_RAM(3)[224], 64);
	dmaCopy(digit7, &BG_TILE_RAM(3)[256], 64);
	dmaCopy(digit8, &BG_TILE_RAM(3)[288], 64);
	dmaCopy(digit9, &BG_TILE_RAM(3)[320], 64);

	initScore();
}

void displayTopSearchFor(int x, int y) {
	int i, j;
	uint8* myMem = (uint8*) BG_GFX_SUB;
	uint8* myNewMem = (uint8*) BG_GFX;

	for(i=0; i < 256; i++) {
		for(j=0; j<192; j++) {
			if( (i >= x) && (i < (x + SEARCH_WIDTH)) && (j >= y) && (j < (y + SEARCH_HEIGHT)) ) {
				swiCopy(myMem + (i+256*j), myNewMem + (i-x+108)+256*(j-y+76), 1);
			}
		}
	}

}

void displayTopTimeBar(int timeLeft) {
	int i, j;
	uint8* myNewMem = (uint8*) BG_GFX;
	uint8 barColour[2] = {2, 2};
	uint8 trans[2] = {0, 0};
	for(i=50; i < (50 + timeLeft); i++) {
		for(j=160; j<180; j++) {
			swiCopy(&barColour, myNewMem + (i+256*j), 1);
		}
	}
	for(i=50 + timeLeft; i < (50 + MAX_TIME); i++) {
		for(j=160; j<180; j++) {
			swiCopy(&trans, myNewMem + (i+256*j), 1);
		}
	}

}


void initTimer() {
	// Initialize timer_ticks
	timer_ticks = 0;

	// Configure timer to trigger an interrupt every 100 ms
	TIMER_DATA(0) = TIMER_FREQ_1024 (8);
	TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1024 | TIMER_IRQ_REQ;

	// Associate the ISR (timerISR) to the interrupt line and enable it
	irqSet(IRQ_TIMER0, &timerISR);
	irqEnable(IRQ_TIMER0);
}

void timerISR() {
	// Disable the timer when 1.5 seconds have passed and call the function
	// playerLoses() to finish the game (player did not play on time)
	timer_ticks++;
	if(timer_ticks >= MAX_TIME) {
		timer_ticks = 0;
		displayNextBottomScreen();
		if(sound) {
			//mmEffect(SFX_APPLAUSE);
			playNextMusic();
		}
		searchForX = rand()%(256 - SEARCH_WIDTH);
		searchForY = rand()%(192 - SEARCH_HEIGHT);
		displayTopSearchFor(searchForX, searchForY);
	}
	displayTopTimeBar(MAX_TIME - timer_ticks);
}

void initSound() {
	mmInitDefaultMem((mm_addr)soundbank_bin);
	mmLoadEffect(SFX_BOING);
	//mmLoadEffect(SFX_APPLAUSE);

	mapMusic[0] = MOD_AURORA;
	mapMusic[1] = MOD_ISLAND;
	mapMusic[2] = MOD_MEDIEVAL;
	mapMusic[3] = MOD_OCEAN;
	mapMusic[4] = MOD_SILLY;
}

void playNextMusic() {
	mmLoad(mapMusic[currentBottomMap]);
	mmStart(mapMusic[currentBottomMap], MM_PLAY_LOOP);
}

void initScore() {
	score = -1;
	updateScore();
}

void updateScore() {
	int units;
	int tens;

	score++;

	units = score%10;
	tens = score/10;

	if(tens) {
		BG_MAP_RAM(25)[15+32*17] = tens + 1;
	} else {
		BG_MAP_RAM(25)[15+32*17] = 0;
	}
	BG_MAP_RAM(25)[16+32*17] = units + 1;
}

void changeCursor(int down, int right) {
	//Clear last cursor
	int i;
	uint8* myNewMem = (uint8*) BG_BMP_RAM_SUB(3);
	uint8 trans[2] = {0, 0};
	for(i=0; i<CURSOR_SIZE; i++) {
		swiCopy(&trans, myNewMem + ((cursorX - CURSOR_SIZE/2 + i) + 256*cursorY), 1);
		swiCopy(&trans, myNewMem + ((cursorX - CURSOR_SIZE/2 + i) + 256*(cursorY-1)), 1);
		swiCopy(&trans, myNewMem + ((cursorX - CURSOR_SIZE/2 + i) + 256*(cursorY+1)), 1);
		swiCopy(&trans, myNewMem + ((cursorX - CURSOR_SIZE/2 + i) + 256*(cursorY-2)), 1);
		swiCopy(&trans, myNewMem + (cursorX + 256*(cursorY - CURSOR_SIZE/2 + i)), 1);
		swiCopy(&trans, myNewMem + ((cursorX-1) + 256*(cursorY - CURSOR_SIZE/2 + i)), 1);
	}
	//Update new cursor position
	int minX = CURSOR_SIZE/2-1;
	int maxX = 256-CURSOR_SIZE/2;
	int minY = CURSOR_SIZE/2-1;
	int maxY = 192-CURSOR_SIZE/2;
	int newX = cursorX + CURSOR_MOVE_FACTOR * right;
	int newY = cursorY + CURSOR_MOVE_FACTOR * down;
	if(newX >= minX && newX <= maxX) {
		cursorX = newX;
	}
	if(newY >= minY && newY <= maxY) {
		cursorY = newY;
	}

	//Need to sort out the edge cases
	uint8 colour[2] = {3, 3};
	for(i=0; i<CURSOR_SIZE; i++) {
		swiCopy(&colour, myNewMem + ((cursorX - CURSOR_SIZE/2 + i) + 256*cursorY), 1);
		swiCopy(&colour, myNewMem + ((cursorX - CURSOR_SIZE/2 + i) + 256*(cursorY-1)), 1);
		swiCopy(&colour, myNewMem + ((cursorX - CURSOR_SIZE/2 + i) + 256*(cursorY+1)), 1);
		swiCopy(&colour, myNewMem + ((cursorX - CURSOR_SIZE/2 + i) + 256*(cursorY-2)), 1);
		swiCopy(&colour, myNewMem + (cursorX + 256*(cursorY - CURSOR_SIZE/2 + i)), 1);
		swiCopy(&colour, myNewMem + ((cursorX-1) + 256*(cursorY - CURSOR_SIZE/2 + i)), 1);
	}
}

void initSplashScreen() {
	VRAM_A_CR = VRAM_ENABLE | VRAM_A_MAIN_BG;
	REG_DISPCNT = MODE_5_2D | DISPLAY_BG2_ACTIVE;
	BGCTRL[2] = BG_BMP_BASE(0) | BgSize_B8_256x256;

	bgTransform[2]->xdx = 256;
	bgTransform[2]->ydx = 0;
	bgTransform[2]->xdy = 0;
	bgTransform[2]->ydy = 256;
	bgTransform[2]->dx = 0;
	bgTransform[2]->dy = 0;

	swiCopy(splashBitmap, BG_GFX, splashBitmapLen/2);
	swiCopy(splashPal, BG_PALETTE, splashPalLen/2);
}

void initMainMenu() {
	REG_DISPCNT_SUB = MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG0_ACTIVE;
	VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;
	swiCopy(menuBitmap, BG_GFX_SUB, menuBitmapLen/2);
	swiCopy(menuPal, BG_PALETTE_SUB, menuPalLen/2);
	BGCTRL_SUB[2] = BG_BMP_BASE(0) | BgSize_B8_256x256;

	REG_BG2PA_SUB = 256;
	REG_BG2PB_SUB = 0;
	REG_BG2PC_SUB = 0;
	REG_BG2PD_SUB = 256;

	BGCTRL_SUB[0] = BG_MAP_BASE(25) | BG_TILE_BASE(3) | BG_32x32 | BG_COLOR_256;

	dmaCopy(x0, &BG_TILE_RAM_SUB(3)[0], 64);
	dmaCopy(x1, &BG_TILE_RAM_SUB(3)[32], 64);
	dmaCopy(x2, &BG_TILE_RAM_SUB(3)[64], 64);

	BG_PALETTE_SUB[1] = ARGB16(1,0,0,0);

	int x, y;
	for (x = 0; x < 32; x++){
			for(y = 0; y < 32; y++){
				BG_MAP_RAM_SUB(25)[y * 32 + x] = 0;
			}
		}

	if(sound) {
		mmLoad(MOD_INTRO);
		mmStart(MOD_INTRO, MM_PLAY_LOOP);
	}

	int keys;
	touchPosition touch;
	while(1) {
		scanKeys();
		keys = keysDown();
		if (keys & KEY_TOUCH) {
			touchRead(&touch);
			if (touch.px >= 128 && touch.px < 144 && touch.py >= 144 && touch.py <160) {
				if(sound) {
					mmStop();
				} else {
					mmLoad(MOD_INTRO);
					mmStart(MOD_INTRO, MM_PLAY_LOOP);
				}
				sound = !sound;
			}
			if (touch.px >= 125 && touch.px < 213 && touch.py >= 97 && touch.py <116) {
				state = 1;
				break;
			}
		}
		if(sound) {
			BG_MAP_RAM_SUB(25)[18*32 + 16] = 1;
			BG_MAP_RAM_SUB(25)[18*32 + 17] = 2;
			BG_MAP_RAM_SUB(25)[19*32 + 16] = 2;
			BG_MAP_RAM_SUB(25)[19*32 + 17] = 1;
		} else {
			BG_MAP_RAM_SUB(25)[18*32 + 16] = 0;
			BG_MAP_RAM_SUB(25)[18*32 + 17] = 0;
			BG_MAP_RAM_SUB(25)[19*32 + 16] = 0;
			BG_MAP_RAM_SUB(25)[19*32 + 17] = 0;
		}
		swiWaitForVBlank();
	}
}

void initFinalScreen() {
	REG_DISPCNT_SUB = MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG0_ACTIVE;
	VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;
	BGCTRL_SUB[2] = BG_BMP_BASE(0) | BgSize_B8_256x256;

	REG_BG2PA_SUB = 256;
	REG_BG2PB_SUB = 0;
	REG_BG2PC_SUB = 0;
	REG_BG2PD_SUB = 256;

	swiCopy(finalBitmap, BG_GFX_SUB, finalBitmapLen/2);
	swiCopy(finalPal, BG_PALETTE_SUB, finalPalLen/2);

	BGCTRL_SUB[0] = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(25) | BG_TILE_BASE(3);

	dmaCopy(emptyTile, &BG_TILE_RAM_SUB(3)[0], 64);
	dmaCopy(digit0, &BG_TILE_RAM_SUB(3)[32], 64);
	dmaCopy(digit1, &BG_TILE_RAM_SUB(3)[64], 64);
	dmaCopy(digit2, &BG_TILE_RAM_SUB(3)[96], 64);
	dmaCopy(digit3, &BG_TILE_RAM_SUB(3)[128], 64);
	dmaCopy(digit4, &BG_TILE_RAM_SUB(3)[160], 64);
	dmaCopy(digit5, &BG_TILE_RAM_SUB(3)[192], 64);
	dmaCopy(digit6, &BG_TILE_RAM_SUB(3)[224], 64);
	dmaCopy(digit7, &BG_TILE_RAM_SUB(3)[256], 64);
	dmaCopy(digit8, &BG_TILE_RAM_SUB(3)[288], 64);
	dmaCopy(digit9, &BG_TILE_RAM_SUB(3)[320], 64);

	int units = score%10;
	int tens = score/10;

	if(tens) {
		BG_MAP_RAM_SUB(25)[7+32*9] = tens + 1;
	} else {
		BG_MAP_RAM_SUB(25)[7+32*9] = 0;
	}
	BG_MAP_RAM_SUB(25)[8+32*9] = units + 1;

	if(sound) {
		mmLoad(MOD_INTRO);
		mmStart(MOD_INTRO, MM_PLAY_LOOP);
	}



	int keys;
	touchPosition touch;
	while(1) {
		scanKeys();
		keys = keysDown();
		if (keys & KEY_TOUCH) {
			touchRead(&touch);
			if (touch.px >= 21 && touch.px < 117 && touch.py >= 97 && touch.py <118) {
				state = 1;
				score = 0;
				break;
			}
			if (touch.px >= 21 && touch.px < 117 && touch.py >= 137 && touch.py <159) {
				state = 0;
				break;
			}
		}
		swiWaitForVBlank();
	}
}


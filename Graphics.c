/*
 * Graphics.c
 *
 * Created: 20/12/2012 11:59:59 PM
 *  Author: Michael a.k.a Not that there's anything wrong with that
 */ 

#include "Graphics.h"
#include "LCD.h"
#include <util/delay.h> 
#include <avr/io.h>
#include <math.h>

unsigned char screenBuffer[LCD_BUFFER_SIZE]; // Our screen buffer array
unsigned char terrainBuffer[LCD_BUFFER_SIZE]; // static screen array

void SetPixel(unsigned char x, unsigned char y, unsigned char value){
	if ((x >= 0) && (y >= 0) && (x < LCD_X) && (y < LCD_Y)){
		int position = x + (y/8)*LCD_X;
		int bits = y%8;
		if (value == 0)
			screenBuffer[position] &= ~(1 << bits);
		else if (value == 1)
			screenBuffer[position] |= (1 << bits);
	}
}

void SetByte(unsigned char x, unsigned char row, unsigned char value){
	if ((x >= 0) && (row >= 0) && (x < LCD_X) && (row < 6)){
		int position = x + row*LCD_X;
		screenBuffer[position] = value;
	}
}

void DrawLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, int stiple) {
	// Gradient
	int dx = x2-x1;
	int dy = y2-y1;
	
	// Count of times used
	int cx = 0;
	int cy = 0;
	
	// Step direction
	int sx = 1;
	int sy = 1;
	if(dx < 0)
	{
		sx = -1;
		dx =-dx;
	}
	if(dy < 0)
	{
		sy = -1;
		dy = -dy;
	}
	
	SetPixel(x1,y1,1);	
	while(x1 != x2 || y1 != y2)
	{
		// Figure out which way to step
		if(cx > cy)
		{
			y1 += sy;
			cy += dx;
			if(dx > dy)
				continue;
		}
		else if(cy > cx)
		{
			x1 += sx;
			cx += dy;
			if(stiple != 0)
			{
				int y2 = y1 + 1;
				while(y2 < LCD_Y)
				{
					SetPixel(x1,y2,(x1^y2) & 1);
					y2++;
				}
			}
			if(dy > dx)
				continue;
		}
		else if(dx > dy)
		{
			x1 += sx;
			cx += dy;
			if(stiple != 0)
			{
				int y2 = y1 + 1;
				while(y2 < LCD_Y)
				{
					SetPixel(x1,y2,(x1^y2) & 1);
					y2++;
				}
			}
			if(dy > dx)
				continue;
		}
		else
		{
			y1 += sy;
			cy += dx;
			if(dx >= dy)
				continue;
		}
		SetPixel(x1,y1,1);
	}
}

void PresentBuffer(void) {
	// (Hint: writes every byte of the buffer to the LCD)
	LCDClear();
	LCDPosition(0, 0);	
	int ii = 0;
	for (ii; ii < LCD_BUFFER_SIZE; ii++){
		LCDWrite(LCD_D, screenBuffer[ii]);}
}

void ClearBuffer(void) {
	memset(screenBuffer, 0x00, LCD_BUFFER_SIZE);
}

void ClearTerrainBuffer(void) {
	memset(terrainBuffer, 0x00, LCD_BUFFER_SIZE);
}

void DrawBall(unsigned char x, unsigned char y){
	int length = 2;
	int height = 2;
	int columnCounter;
	int rowCounter;	
	for (columnCounter = x; columnCounter < (x+length); columnCounter++){
		for (rowCounter = y; rowCounter < (y+height); rowCounter++){
			SetPixel(columnCounter, rowCounter, 1);}}
}

/**
* Extend this file with whatever other graphical functions you may need
*	Remember to include the declaration of any new functions in "Graphics.h"
*/

//writes startup terrain using LCDWrite
void Terrain(void) {
	//set position to 5th line from bottom
	LCDPosition(0, 5);

	//writes a solid line spanning 5th line from bottom and then alternate pixels onwards
	int columnCounter;
	for (columnCounter = 0; columnCounter<42; columnCounter++)
	{
		LCDWrite(LCD_D, 0xA8);
		LCDWrite(LCD_D, 0x58);	
	}	
}

//draws terrain using buffer
void DrawTerrain(void) {
	//writes a solid line spanning 5th line from bottom and then alternate pixels onwards
	int columnCounter;
	int rowCounter;
	for (columnCounter = 0; columnCounter<LCD_X; columnCounter++){
		SetPixel(columnCounter, (LCD_Y-5), 1);
	}	
	for (columnCounter = 0; columnCounter<LCD_X; columnCounter += 2){
		for (rowCounter = (LCD_Y-4); rowCounter < LCD_Y; rowCounter += 2){
			SetPixel(columnCounter, rowCounter, 1);
		}
	}	
	for (columnCounter = 1; columnCounter<LCD_X; columnCounter += 2){
		for (rowCounter = (LCD_Y-5); rowCounter < LCD_Y; rowCounter +=2 ){
			SetPixel(columnCounter, rowCounter, 1);
		}
	}	
}

#define STEP 4
int y_coords[LCD_X/STEP];
int PlayerTankX = 5;
int PlayerTankY = 0;
int AITankX = (LCD_X-5);
int AITankY = 0;
void InitTerrain(void)
{
	// Set seed
	srand(1234);

	//allocate array for y-coords, 
	memset(y_coords, 0, LCD_X/STEP);
	
	//pass array to function to populate with values, need 84 to fill screen / 4 pixels per coordinate
	generateTerrainPoints(y_coords, LCD_X/STEP);
	
	// Find tank locations
	PlayerTankY = y_coords[PlayerTankX/STEP];
	if(PlayerTankY > y_coords[PlayerTankX/STEP + 1])
	{
		PlayerTankY = y_coords[PlayerTankX/STEP + 1];
	}
	PlayerTankY -= 2;
	
	AITankY = y_coords[AITankX/STEP];
	if(AITankY > y_coords[AITankX/STEP + 1])
	{
		AITankY = y_coords[AITankX/STEP + 1];
	}
	AITankY -= 2;
	
	// Smooth terrain off around the tanks
	y_coords[PlayerTankX/STEP] = PlayerTankY + 2;
	y_coords[PlayerTankX/STEP+1] = PlayerTankY + 2;
	y_coords[AITankX/STEP] = AITankY + 2;
	y_coords[AITankX/STEP+1] = AITankY + 2;
	
}

void DrawVariableTerrain(void){
	int ix;

	//draw line between points
	for(ix=0; ix<LCD_X; ix+=4){
		DrawLine(ix, y_coords[ix/STEP], ix+STEP, y_coords[ix/STEP+1], 1);
	}
}

int CheckIfUnderTerrain(int x, int y)
{
	int left = y_coords[x/STEP];
	int right = y_coords[x/STEP + 1];
	
	int lerpOffset = x % STEP;
	int yAtX = (right * lerpOffset + left * (STEP - lerpOffset)) / STEP;
	
	if(yAtX < y)
		return 1;
	return 0;
}

#define DAMAGE 3;
void DeformTerrain(int x)
{
	y_coords[x/STEP]++;
	y_coords[x/STEP + 1]++;
}

//draws tanks using buffer
void DrawTanks() {
	//draws a tank 3 pixels tall, 5 wide, with top row only showing centre pixel
	int columnCounter;
	int rowCounter;	
	int length = 5;
	int height = 2;

	//draw tank 1
	for (columnCounter = PlayerTankX - 2; columnCounter <= PlayerTankX + 2; columnCounter++){
		for (rowCounter = PlayerTankY; rowCounter <= PlayerTankY + 1; rowCounter++)	
			SetPixel(columnCounter, rowCounter, 1);
	}
	SetPixel(PlayerTankX, PlayerTankY - 1, 1);
	
	//draw tank 2
	for (columnCounter = AITankX - 2; columnCounter <= AITankX + 2; columnCounter++){
		for (rowCounter = AITankY; rowCounter <= AITankY + 1; rowCounter++)	
			SetPixel(columnCounter, rowCounter, 1);
	}
	SetPixel(AITankX, AITankY - 1, 1);
}

//draw barrel of tank to buffer
// Angle is an integer of degrees required to be between 10 and 80 inclusive
void DrawBarrel(int angle, int power){
	int s = (double)power * sin((double)angle /180.0 * M_PI);
	int c = (double)power * cos((double)angle /180.0 * M_PI);
	
	DrawLine(PlayerTankX, PlayerTankY - 2, PlayerTankX + c, PlayerTankY - s - 2, 0);
}

//draws barrel of enemy tank
void AIDrawBarrel(int angle, int power){
	int s = (double)power * sin((double)angle /180.0 * M_PI);
	int c = (double)power * cos((double)angle /180.0 * M_PI);
	
	DrawLine(AITankX, AITankY - 2, AITankX - c, AITankY - s - 2, 0);
}

//draws barrel of enemy tank for PART A
void AIDrawBarrelA(int angle, int power){

	angle = (angle+1)*10;


	int s = (double)power * sin((double)angle /180.0 * M_PI);
	int c = (double)power * cos((double)angle /180.0 * M_PI);
	
	DrawLine(LCD_X-7, LCD_Y-9, LCD_X-7- c, LCD_Y-9-s, 0);
}

//draw splash animation for shell hit
void DrawSplash(unsigned char x, unsigned char y, int dist){
	DrawVariableTerrain();
	DrawTanks();		
	SetPixel(x, y-6, 1);
	SetPixel(x+1, y-6-dist, 1);
	SetPixel(x-1, y-6-dist, 1);
	SetPixel(x+3, y-6-dist*2, 1);
	SetPixel(x-3, y-6-dist*2, 1);	
	PresentBuffer();
	_delay_ms(50);	
}

//calculate wind speed based on ADC result
int WindSpeed(int result){
	// the wind goes from 0 (full right) to 1024 (full left).
	int wind;
	wind = result / 114 - 4;
	LCDPosition(70,0);
	LCDString("  ");
	LCDPosition(70, 0);	
	dbgI(wind, 1);	
	return wind;	
}


//writes startup tanks using LCDWrite
void Tanks(void) {
	//sets position of first tank
	LCDPosition(3, 5);

	//draws a tank 3 pixels tall, 5 wide, with top row only showing centre pixel
	LCDWrite(LCD_D, 0x5E);
	LCDWrite(LCD_D, 0xAE);
	LCDWrite(LCD_D, 0x5F);
	LCDWrite(LCD_D, 0xAE);
	LCDWrite(LCD_D, 0x5E);		
	
	//sets position of second tank
	LCDPosition(75, 5);

	//draws a tank 3 pixels tall, 5 wide, with top row only showing centre pixel
	LCDWrite(LCD_D, 0x5E);
	LCDWrite(LCD_D, 0xAE);
	LCDWrite(LCD_D, 0x5F);
	LCDWrite(LCD_D, 0xAE);
	LCDWrite(LCD_D, 0x5E);	
}


//writes barrel
void TankBarrel(int angle) {
	//sets first position
	LCDPosition(5, 4);		//above centre of tank

	//clears previous barrel
	int columnClear;
	for (columnClear = 0; columnClear<8; columnClear++){
		LCDWrite(LCD_D, 0x00);}		

	//resets first position
	LCDPosition(5, 4);		//above centre of tank		
		
	//draws the next pixels
	int columnCounter;
	if (angle == 1){
		for (columnCounter = 0; columnCounter<3; columnCounter++)
			LCDWrite(LCD_D, 0x80);
		for (columnCounter = 0; columnCounter<2; columnCounter++)
			LCDWrite(LCD_D, 0x40);}
	else if(angle == 2){
		LCDWrite(LCD_D, 0x80);
		LCDWrite(LCD_D, 0x80);		
		LCDWrite(LCD_D, 0x40);
		LCDWrite(LCD_D, 0x40);	
		LCDWrite(LCD_D, 0x20);}
	else if(angle == 3){
		LCDWrite(LCD_D, 0x80);
		LCDWrite(LCD_D, 0x80);		
		LCDWrite(LCD_D, 0x40);
		LCDWrite(LCD_D, 0x20);	
		LCDWrite(LCD_D, 0x10);}	
	else if(angle == 4){
		LCDWrite(LCD_D, 0x80);
		LCDWrite(LCD_D, 0x40);		
		LCDWrite(LCD_D, 0x20);
		LCDWrite(LCD_D, 0x10);	
		LCDWrite(LCD_D, 0x08);}		
	else if(angle == 5){
		LCDWrite(LCD_D, 0xC0);
		LCDWrite(LCD_D, 0x30);		
		LCDWrite(LCD_D, 0x08);}		
	else if(angle == 6){
		LCDWrite(LCD_D, 0xE0);
		LCDWrite(LCD_D, 0x18);}
	return;	
}

//writes barrel's power level
void BarrelPower(int power, int angle) {
	//horizontal clear
	if (power == 5){		//resets for full power at given angle
		TankBarrel(angle);}
	if ((angle < 5) && (power == 4)){	
		LCDPosition(9, 4);	
		LCDWrite(LCD_D, 0x00);}		
	if ((angle < 5) && (power == 3)){	
		LCDPosition(8, 4);	
		LCDWrite(LCD_D, 0x00);}
	if ((angle < 5) && (power == 2)){	
		LCDPosition(7, 4);	
		LCDWrite(LCD_D, 0x00);}	
	if ((angle < 5) && (power == 1)){	
		LCDPosition(6, 4);	
		LCDWrite(LCD_D, 0x00);}				
	
	//vertical clear
	if ((angle == 5) && (power == 4)){	
		LCDPosition(7, 4);	
		LCDWrite(LCD_D, 0x00);}	
	if ((angle == 5) && (power == 3)){	
		LCDPosition(6, 4);
		LCDWrite(LCD_D, 0x20);}
	if ((angle == 5) && (power == 2)){	
		LCDPosition(6, 4);
		LCDWrite(LCD_D, 0x00);}		
	if ((angle == 5) && (power == 1)){	
		LCDPosition(5, 4);
		LCDWrite(LCD_D, 0x80);}
	if ((angle == 6) && (power == 4)){	
		LCDPosition(6, 4);
		LCDWrite(LCD_D, 0x10);}			
	if ((angle == 6) && (power == 3)){	
		LCDPosition(6, 4);
		LCDWrite(LCD_D, 0x00);}	
	if ((angle == 6) && (power == 2)){	
		LCDPosition(5, 4);
		LCDWrite(LCD_D, 0xC0);}		
	if ((angle == 6) && (power == 1)){	
		LCDPosition(5, 4);
		LCDWrite(LCD_D, 0x80);}			
	return;	
}

//draws terrain using buffer
void DrawTerrainA(void) {
	//writes a solid line spanning 5th line from bottom and then alternate pixels onwards
	int columnCounter;
	int rowCounter;
	for (columnCounter = 0; columnCounter<LCD_X; columnCounter++){
		SetPixel(columnCounter, (LCD_Y-5), 1);
	}	
	for (columnCounter = 0; columnCounter<LCD_X; columnCounter += 2){
		for (rowCounter = (LCD_Y-4); rowCounter < LCD_Y; rowCounter += 2){
			SetPixel(columnCounter, rowCounter, 1);
		}
	}	
	for (columnCounter = 1; columnCounter<LCD_X; columnCounter += 2){
		for (rowCounter = (LCD_Y-5); rowCounter < LCD_Y; rowCounter +=2 ){
			SetPixel(columnCounter, rowCounter, 1);
		}
	}	
}


//draws tanks using buffer
void DrawTanksA(void) {
	//draws a tank 3 pixels tall, 5 wide, with top row only showing centre pixel
	int columnCounter;
	int rowCounter;	
	int length = 5;
	int height = 2;

	//draw tank 1
	int xaxis1 = 3;
	int yaxis1 = 6;
	for (columnCounter = xaxis1; columnCounter < (xaxis1+length); columnCounter++){
		for (rowCounter = (LCD_Y-yaxis1); rowCounter > (LCD_Y-yaxis1-height); rowCounter--)	
			SetPixel(columnCounter, rowCounter, 1);
	}
	SetPixel(xaxis1+2, LCD_Y-yaxis1-height, 1);
	
	//draw tank 2
	int xaxis2 = (LCD_X-3-5);
	int yaxis2 = 6;
	for (columnCounter = xaxis2; columnCounter < (xaxis2+length); columnCounter++){
		for (rowCounter = (LCD_Y-yaxis2); rowCounter > (LCD_Y-yaxis2-height); rowCounter--)	
			SetPixel(columnCounter, rowCounter, 1);
	}
	SetPixel(xaxis2+2, LCD_Y-yaxis2-height, 1);
}

void DrawBallA(unsigned char x, unsigned char y){
	int length = 2;
	int height = 2;
	int columnCounter;
	int rowCounter;	
	for (columnCounter = x; columnCounter < (x+length); columnCounter++){
		for (rowCounter = y; rowCounter < (y+height); rowCounter++){
			SetPixel(columnCounter, rowCounter, 1);}}
}

void DrawSplashA(unsigned char x, unsigned char y, int dist){
	DrawTerrainA();
	DrawTanksA();		
	SetPixel(x, LCD_Y-6, 1);
	SetPixel(x+1, LCD_Y-6-dist, 1);
	SetPixel(x-1, LCD_Y-6-dist, 1);
	SetPixel(x+3, LCD_Y-6-dist*2, 1);
	SetPixel(x-3, LCD_Y-6-dist*2, 1);	
	PresentBuffer();
	_delay_ms(50);	
}
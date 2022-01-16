/*
 * Graphics.h
 *
 * Created: 20/12/2012 11:59:59 PM
 *  Author: Michael a.k.a Mickey Mouse
 */ 


#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "LCD.h"
#include <string.h>

// The size of the buffer we need to represent the LCD RAM
#define LCD_BUFFER_SIZE LCD_X * (LCD_Y / 8)
#define boxsize 14
 
// We declare the screen buffer here as extern, since it is defined in our C file.
// This way other files have will have direct access to the buffer if they include Graphics.h
extern unsigned char screenBuffer[LCD_BUFFER_SIZE];

extern int PlayerTankX;
extern int PlayerTankY;
extern int AITankX;
extern int AITankY;

void SetPixel(unsigned char x, unsigned char y, unsigned char value);
void SetByte(unsigned char x, unsigned char row, unsigned char value);
void DrawLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, int stiple);
void PresentBuffer(void);
void ClearBuffer(void);
void ClearTerrainBuffer(void);
void IntroScreen(void);
void Starup(void);
void winSequence(void);
void loseSequence(void);
void DrawTerrain(void);
void InitTerrain(void);
void DrawVariableTerrain(void);
int CheckIfUnderTerrain(int x, int y);
void DeformTerrain(int x);
void TankBarrel(int angle);
void DrawBarrel(int angle, int power);
void AIDrawBarrel(int angle, int power);
void DrawSplash(unsigned char x, unsigned char y, int dist);
int WindSpeed(int result);
void DrawSplashA(unsigned char x, unsigned char y, int dist);
void DrawBallA(unsigned char x, unsigned char y);
void DrawTanksA(void);
void DrawTerrainA(void);
void BarrelPower(int power, int angle);
void TankBarrel(int angle);
void Tanks(void);


#endif /* GRAPHICS_H_ */
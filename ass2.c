/**
* Following includes:
*	- avr/io.h includes the port and pin definitions (i.e. DDRB, PORTB, PB1, etc)
*	- util/delay.h provides the _delay_ms function which is very useful
*	- LCD.h includes the LCD.c functions
*	- Graphics.h includes the Graphics.c functions
*/
#include <avr/io.h>
#include <util/delay.h> 
#include "LCD.h" 
#include "Graphics.h"
#include <avr/interrupt.h>
#include <math.h>

/**
* Function declarations
*/
void init();

/**
* Variable declarations
*/

volatile unsigned int result;

#define WIND_FACTOR 10
/**
* Main function where code operations with the libraries should be performed
*/
int main() {

//		//initialization 
	CPU_PRESCALE(0);	
	init();
	
	//initialize variables
	int startupPlayed = 0;
	int winCondition = 0;
	int setting;
	int initialRound;
	int roundCount = 0;
	int wind = 0;
	int AIy;
	int AIx;
	int x;
	int y;
	int gravity;
	int multiplier;	
	int ii;
	int iw;
	int dist;
	int roundcount;
	int windSpeed;
	int angle = 10;
	int power = 10;
	int AIpower = 5;
	int AIangle = 10;
	int GameA = 0;											//reset game choice
	
	//reset round counter
	initialRound = 1;
	
	//display string with title, name and student number
	IntroScreen();
	_delay_ms(1000);
	
		
	LCDClear();
	LCDPosition(1, 5);
	LCDString("A");
	LCDPosition(76, 5);
	LCDString("B");	
	while(1){
	
		if (PINB & 1){	//if A is selected
				
					_delay_ms(100);	//wait for 50ms
					if (PINB & 1){	//if still pressed
						GameA = 1;							//select game A
						break;
					}
		}
		
		if (PINB & 2){	//if B is selected
				
					_delay_ms(100);	//wait for 50ms
					if (PINB & 2){	//if still pressed		
					GameA = 0;								//select game B
					break;
					}
		}
	}


			
//	GAME B
	
	while(1)
	{
		//if player wants game A, skip game B
		if (GameA == 1)
			break;
		
		IntroScreen();
	
		//if button 1 is pressed(debounced), start game
		while(1)
		{					
			if (PINB & 1)	//if pressed
			{
				_delay_ms(100);	//wait for 50ms
				if (PINB & 1)	//if still pressed
				{

					//display startup countdown if not already played
					if (startupPlayed == 0)
					{
//						Starup();
						startupPlayed = 1;
					}

					if (winCondition ==1){	//if player has won, reset to start screen
						winCondition = 0;	
						break;
						}
					
					// Reset Power
					power = 10;
					
					//clear LCD for next screen
					LCDClear();		
					
					//display terrain and tanks
					DrawVariableTerrain();
					DrawTanks();
					DrawBarrel(angle, power);						
					PresentBuffer();
					windSpeed = WindSpeed(result);					

					//wait for user to push button to select angle
					while(1)
					{
						//user selects angle
						LCDPosition(1, 1);
						LCDString("Choose angle");
						windSpeed = WindSpeed(result);					
						if (PINB & 1)	//user presses button 1 to cycle through
						{
							_delay_ms(100);	//wait for 50ms
							if (PINB & 1)	//if still pressed
							{						
								if (angle < 80)
									angle+=10;	
								else
									angle = 10;	//reset angle to lowest
							}
							ClearBuffer();
							DrawVariableTerrain();
							DrawTanks();
							DrawBarrel(angle, power);					
							PresentBuffer();							
						}
						
						if (PINB & 2)	//user presses button 2 to select
						{
							_delay_ms(100);	//wait for 50ms
							if (PINB & 2)	//if still pressed	
								break;
						}
					}
					
					//wait for user to push button to select power				
					while(1)
					{	
						//user selects power
						LCDPosition(1, 1);
						LCDString("Choose power");
						windSpeed = WindSpeed(result);	
						if (PINB & 1)	//if button 1 is pressed
						{
							_delay_ms(100);	//wait for 50ms
							if (PINB & 1)	//if still pressed
							{						
								if (power > 1)
									power--;	
								else
									power = 10;	//reset power to highest
							}					
							ClearBuffer();
							DrawVariableTerrain();
							DrawTanks();
							DrawBarrel(angle, power);						
							PresentBuffer();				
						}
						
						if (PINB & 2)	//user presses button 2 to select
						{
							_delay_ms(100);	//wait for 50ms
							if (PINB & 2)	//if still pressed	
								break;
						}					
					}
				
					//power and angle are selected, get ready to fire shot!
					LCDPosition(1, 1);
					LCDString("Firing shot!");
					windSpeed = WindSpeed(result);	
					_delay_ms(500);
					LCDClear();					
					
					//set ball starting position
					x = PlayerTankX;
					y = PlayerTankY-3;
					gravity = 1;
					multiplier = 1;
					DrawVariableTerrain();
					DrawTanks();	
					DrawBall(x, y);	
					DrawBarrel(angle, power);						
					PresentBuffer();					

					int vy = WIND_FACTOR * -(double)power * sin((double)angle /180.0 * M_PI);
					int vx = WIND_FACTOR * (double)power * cos((double)angle /180.0 * M_PI);
					
					//draw bullet trajectory				
					while (CheckIfUnderTerrain(x, y) == 0){
						wind = WindSpeed(result);
						LCDPosition(70, 0);	
						dbgI(wind, 1);	
							DrawVariableTerrain();
						DrawTanks();		
						DrawBall(x, y);
						PresentBuffer();
						_delay_ms(50);	
						ClearBuffer();
						x = x + vx / WIND_FACTOR;
						y = y + vy / WIND_FACTOR;
						vx = vx + wind;
						vy = vy + gravity * WIND_FACTOR;
						
						if(x > 84)
						{
							break;
						}
						if(x < 0)
						{
							break;
						}
					}

					//bullet explosion/splash animation
					ii = 0;
					dist = 3;
					for(ii; ii<dist; ii++){
						DrawSplash(x, y, ii);
						ClearBuffer();	
					}
					
					DeformTerrain(x);
					
					//if bullet lands on tank
					if((x >= LCD_X-3-5-1) && (x < LCD_X-2)){				
						winSequence();		//game over screen and LED flashes
						winCondition = 1;	//set win condition so program returns to start screen
						roundCount = 0;	//reset round counter
						break;
					}
					else{
						LCDPosition(1, 1);					
						windSpeed = WindSpeed(result);	
						LCDString("   Missed!  ");
						_delay_ms(500);
						}
			
					

					
					//ENEMY FIRING!
			
					
					//AI - create new seed
					seedWithButtonPress();	
								
					//AI - if first round: start with random power and angle
					if(initialRound == 1){
						AIpower = randInRange(1, 5);
						AIangle = randInRange(1, 6);
						initialRound = 0;
						DrawVariableTerrain();
						AIDrawBarrel(AIangle, AIpower);	
						DrawTanks();
						PresentBuffer();						
					}	
					//AI - else set either power or angle (setting decided randomly) power=1, angle=2
					else
						setting = (rand() & 1) + 1; // randInRange(1, 2);
		
					//show AI's last trajectory
					DrawVariableTerrain();
					AIDrawBarrel(AIangle, AIpower);	
					DrawTanks();
					PresentBuffer();
					_delay_ms(500);						
		
					//AI - based on last shot, adjust either power or angle to a more appropriate value				
					if(setting == 1){
						if(AIx < 3){
							AIpower--;
							if(AIpower<1){
								AIpower++;
								AIangle--;
								if(AIangle<1){
									AIangle++;
									AIpower = 5;
								}	
							}		
						}							
						else if(AIx > 3){
							AIpower++;	
							if(AIpower>5){
								AIpower--;
								AIangle++;
								if(AIangle>6){
									AIangle--;
									AIpower = 1;
								}	
							}	
						}		
					}
					else if(setting == 2){
						if(AIx < 3){
							AIangle--;					
							if(AIangle>6){
								AIangle--;
								AIpower++;
								if(AIpower>5){
									AIpower--;
									AIangle = 1;
								}
							}	
						}		
						else if(AIx > 3){
							AIangle++;						
							if(AIangle<1){
								AIangle++;
								AIpower--;
								if(AIpower<1){
									AIpower++;
									AIangle = 6;
								}
							}	
						}	
					}

					//show adjusted trajectory
					DrawVariableTerrain();
					AIDrawBarrel(AIangle, AIpower);	
					DrawTanks();
					PresentBuffer();
					ClearBuffer();
					
//					//AI - power and angle are selected, get ready to fire shot!
					LCDPosition(1, 1);
					LCDString(" AI Firing! ");
					windSpeed = WindSpeed(result);	
					_delay_ms(500);
						
					//AI - set ball starting position
					AIx = AITankX;
					AIy = AITankY-3;

					int AIvy = WIND_FACTOR * -(double)AIpower * sin((double)AIangle /180.0 * M_PI);
					int AIvx = WIND_FACTOR * -(double)AIpower * cos((double)AIangle /180.0 * M_PI);
					
					//AI - draw bullet trajectory
					multiplier = 1;
					while (CheckIfUnderTerrain(AIx, AIy) == 0){					
						windSpeed = WindSpeed(result);
						LCDPosition(70, 0);	
						dbgI(windSpeed, 1);
						DrawVariableTerrain();
						DrawTanks();		
						DrawBall(AIx, AIy);
						PresentBuffer();
						_delay_ms(50);	
						ClearBuffer();
						
						AIx = AIx + AIvx / WIND_FACTOR;
						AIy = AIy + AIvy / WIND_FACTOR;
						AIvx = AIvx + wind;
						AIvy = AIvy + gravity * WIND_FACTOR;	
					}
					
					//AI - bullet explosion/splash animation
					ii = 0;
					dist = 3;
					for(ii; ii<dist; ii++){
						DrawSplash(AIx, AIy, ii);
						ClearBuffer();	
					}
										
					//AI - if bullet lands on tank
					if((AIx >= 3) && (AIx <= 9)){
						//game over screen and LED flashes
						loseSequence();
						winCondition = 1;	//set win condition so program returns to start screen
						roundCount = 0;	//reset round counter
						break;
					}
					else{
						LCDPosition(1, 1);					
						LCDString(" AI Missed! ");
						_delay_ms(500);
						LCDPosition(1, 1);	
						LCDString(" Your Turn! ");	
						roundCount++;
					}					
				} 
			}
		}		
	}	
	
//	GAME A
	
	while(1)
	{
		IntroScreen();
		
		while(1)
		{
		
		
		
		
		// player wants game A
		if (GameA == 2)
			break;	
	
			if (PINB & 1)	//if pressed
			{
				_delay_ms(100);	//wait for 50ms
				if (PINB & 1)	//if still pressed
				{	

					//display startup countdown if not already played
					if (startupPlayed == 0)
					{
						Starup();
						startupPlayed = 1;
					}
					
					if (winCondition ==1){	//if player has won, reset to start screen
						winCondition = 0;	
						break;
						}
					
					//clear LCD for next screen
					LCDClear();		

					//display terrain and tanks
					Terrain();
					Tanks();
					
					//write tank barrel, start with lowest angle
					int angle = 1;
					TankBarrel(angle);
	
					//wait for user to push button to select angle
					while(1)
					{
						//user selects angle
						LCDPosition(1, 1);
						LCDString("Choose angle");
						if (PINB & 1)	//user presses button 1 to cycle through
						{
							_delay_ms(100);	//wait for 50ms
							if (PINB & 1)	//if still pressed
							{						
								if (angle < 6)
									angle++;	
								else
									angle = 1;	//reset angle to lowest
							}
						TankBarrel(angle);		//update angle
						}
						
						if (PINB & 2)	//user presses button 2 to select
						{
							_delay_ms(100);	//wait for 50ms
							if (PINB & 2)	//if still pressed	
								break;
						}
					}
								
					//wait for user to push button to select power				
					int power = 5;
					while(1)
					{
						//user selects power
						LCDPosition(1, 1);
						LCDString("Choose power");

						if (PINB & 1)	//if button 1 is pressed
						{
							_delay_ms(100);	//wait for 50ms
							if (PINB & 1)	//if still pressed
							{						
								if (power > 1)
									power--;	
								else
									power = 5;	//reset power to highest
							}
						BarrelPower(power, angle);		//update power
						}
						
						if (PINB & 2)	//user presses button 2 to select
						{
							_delay_ms(100);	//wait for 50ms
							if (PINB & 2)	//if still pressed	
								break;
						}					
					}
					
					//power and angle are selected, get ready to fire shot!
					LCDPosition(1, 1);
					LCDString("Firing shot!");
					_delay_ms(500);
					LCDClear();
						
					//set ball starting position
					int x = 5;
					int y = (LCD_Y-10);
					int gravity = 1;
					int multiplier = 1;
					DrawTerrainA();
					DrawTanksA();		
					DrawBallA(x, y);					
					PresentBuffer();					

					//draw bullet trajectory					
					while (y < (LCD_Y-5)){
						DrawTerrainA();
						DrawTanksA();		
						DrawBallA(x, y);
						PresentBuffer();
						_delay_ms(50);	
						ClearBuffer();
						x = x + power*1.5;
						y = y - power - angle + gravity*multiplier;
						multiplier+= 1;
					}
					
					//bullet explosion/splash animation
					int ii = 0;
					int dist = 3;
					for(ii; ii<dist; ii++){
						DrawSplashA(x, y, ii);
						ClearBuffer();	
					}
					
					//if bullet lands on tank
					if((x >= LCD_X-3-5-1) && (x < LCD_X-2)){
						//game over screen and LED flashes
						winSequence();
						winCondition = 1;	//set win condition so program returns to start screen
						int roundCount = 0;	//reset round counter
						break;
					}
					else{
						LCDPosition(1, 1);					
						LCDString("   Missed!  ");
						_delay_ms(500);
						}
			
					

					
					//ENEMY FIRING!

					//AI - create new seed
					seedWithButtonPress();	
					int AIpower;
					int AIangle;
					int AIy;
					int AIx;
								
					//AI - if first round: start with random power and angle
					if(initialRound == 1){
						AIpower = randInRange(1, 5);
						AIangle = randInRange(1, 6);
						initialRound = 0;					
					}				
					//AI - choose to adjust power or angle (setting decided randomly) power=1, angle=2
					else{
						setting = (rand() & 1) + 1; // randInRange(1, 2);
					}
					
					//show AI's last trajectory
					DrawTerrainA();
					AIDrawBarrelA(AIangle, AIpower);	
					DrawTanksA();		
					PresentBuffer();
					ClearBuffer();
					_delay_ms(500);						
					
					//AI - based on last shot, adjust either power or angle to a more appropriate value
					if(setting == 1){
						if(AIx < 3){
							AIpower--;
							if(AIpower<1){
								AIpower++;
								AIangle--;
								if(AIangle<1){
									AIangle++;
									AIpower = 5;
								}	
							}		
						}							
						else if(AIx > 3){
							AIpower++;	
							if(AIpower>5){
								AIpower--;
								AIangle++;
								if(AIangle>6){
									AIangle--;
									AIpower = 1;
								}	
							}	
						}		
					}
					else if(setting == 2){
						if(AIx < 3){
							AIangle--;					
							if(AIangle>6){
								AIangle--;
								AIpower++;
								if(AIpower>5){
									AIpower--;
									AIangle = 1;
								}
							}	
						}		
						else if(AIx > 3){
							AIangle++;						
							if(AIangle<1){
								AIangle++;
								AIpower--;
								if(AIpower<1){
									AIpower++;
									AIangle = 6;
								}
							}	
						}	
					}
					//show adjusted trajectory
					DrawTerrainA();
					AIDrawBarrelA(AIangle, AIpower);	
					DrawTanksA();
					PresentBuffer();
					ClearBuffer();
					_delay_ms(500);
						
					//AI - power and angle are selected, get ready to fire shot!
					DrawTerrainA();
					DrawTanksA();	
					PresentBuffer();	
					ClearBuffer();
					LCDPosition(1, 1);
					LCDString(" AI Firing! ");
					_delay_ms(500);
						
					//AI - set ball starting position
					AIx = LCD_X - 7;
					AIy = (LCD_Y-10);
					DrawTerrainA();
					DrawTanksA();		
					DrawBallA(AIx, AIy);
					PresentBuffer();					
					
					//AI - draw bullet trajectory
					multiplier = 1;
					while (AIy < (LCD_Y-5)){			
						DrawTerrainA();
						DrawTanksA();		
						DrawBallA(AIx, AIy);
						PresentBuffer();
						_delay_ms(50);	
						ClearBuffer();
						AIx = AIx - AIpower*1.5;
						AIy = AIy - AIpower - AIangle + gravity*multiplier;
						multiplier+= 1;						
					}
					
					//AI - bullet explosion/splash animation
					ii = 0;
					dist = 3;
					for(ii; ii<dist; ii++){
						DrawSplashA(AIx, AIy, ii);
						ClearBuffer();	
					}
										
					//AI - if bullet lands on tank
					if((AIx >= 3) && (AIx <= 9)){
						//game over screen and LED flashes
						loseSequence();
						winCondition = 1;	//set win condition so program returns to start screen
						roundCount = 0;	//reset round counter
						break;
					}
					else{
						LCDPosition(1, 1);					
						LCDString(" AI Missed! ");
						_delay_ms(500);
						LCDPosition(1, 1);	
						LCDString(" Your Turn! ");	
						roundCount++;
						_delay_ms(500);						
					}					
				} 
			}
		}	
	}	
		
	return 0;
}
		
	

/**
* Function to initialise the Teensy environment. It should do the following:
*	- Correctly initialise the LCD screen
*	- Configure any required pins to inputs and outputs
* Segmenting common pieces of code like this into functions is good practice!
*/

void init() 
{
	//LCD initialization
	LCDInitialise(LCD_DEFAULT_CONTRAST);	//set contrast
	LCDClear();								//clear LCD
	
	//DDR and Port setup
	DDRB |=  0b00001100; 					//set LEDs as outputs and buttons as inputs
	PORTB |= 0b00000000;					//set LEDs to off
	
	// ADC reference voltage to AVCC
	ADMUX |= (1 << REFS0);
	
	// Prescalar to 128
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	// Enable ADC
	ADCSRA |= (1 << ADEN);
	
	// Enable ADC interrupts
	ADCSRA |= (1 << ADIE);
	
	// Listen to interrupets
	sei();
	
	// Set ADC to run
	ADCSRA |= (1<<ADSC);	
	
	// Init terrain data
	InitTerrain();
}	


//interrupt vector for ADC
ISR(ADC_vect){
	// Copy data out
	unsigned char L = ADCL;
	unsigned char H = ADCH;
	result = (unsigned int)L | (((unsigned int)H) << 8);
	
	// Set ADC to run
	ADCSRA |= (1<<ADSC);
}




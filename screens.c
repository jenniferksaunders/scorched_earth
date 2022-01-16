#include "Graphics.h"
#include "LCD.h"
#include <util/delay.h> 
#include <avr/io.h>

//first screen a user sees
void IntroScreen(void){
	LCDClear();
	LCDPosition(13, 0);
	LCDString("Scorched");
	LCDPosition(23, 1);
	LCDString("Earth");
	LCDPosition(13, 3);
	LCDString("Jennifer");
	LCDPosition(13, 4);
	LCDString("Saunders");
	LCDPosition(13, 5);
	LCDString("n8805555");	
}

//writes startup sequence
void Starup(void){
	LCDClear();	
	PORTB ^= (1<<3); //toggle LED
	PORTB ^= (1<<2); //toggle LED	
	LCDPosition(1, 1);
	LCDString("Game Starts:");	//display Game Start text
	LCDPosition(35, 3);
	LCDCharacter('3');	//display 3
	_delay_ms(500);
	PORTB ^= (1<<3); //toggle LED
	PORTB ^= (1<<2); //toggle LED
	_delay_ms(500);				
	PORTB ^= (1<<3); //toggle LED
	PORTB ^= (1<<2); //toggle LED
	LCDPosition(35, 3);
	LCDCharacter('2');	//display 2
	_delay_ms(500);
	PORTB ^= (1<<3); //toggle LED
	PORTB ^= (1<<2); //toggle LED
	_delay_ms(500);		
	PORTB ^= (1<<3); //toggle LED
	PORTB ^= (1<<2); //toggle LED
	LCDPosition(35, 3);
	LCDCharacter('1');	//display 1
	_delay_ms(500);
	PORTB ^= (1<<3); //toggle LED
	PORTB ^= (1<<2); //toggle LED
	LCDClear();
	LCDPosition(32, 1);	
	LCDString("Go!");	//display 1
	_delay_ms(500);
}
	
void winSequence(void){
	LCDPosition(1, 1);
	LCDString(" Enemy hit! ");
	PORTB ^= (1<<3);
	PORTB ^= (1<<2);						
	_delay_ms(100);
	PORTB ^= (1<<3);
	PORTB ^= (1<<2);
	_delay_ms(100);
	PORTB ^= (1<<3);
	PORTB ^= (1<<2);
	_delay_ms(100);
	PORTB ^= (1<<3);
	PORTB ^= (1<<2);						
	_delay_ms(100);
}

void loseSequence(void){
	LCDPosition(1, 1);
	LCDString("  You Lose! ");
	PORTB ^= (1<<3);
	PORTB ^= (1<<2);
	_delay_ms(100);
	PORTB ^= (1<<3);
	PORTB ^= (1<<2);
	_delay_ms(100);
	PORTB ^= (1<<3);
	PORTB ^= (1<<2);
	_delay_ms(100);
	PORTB ^= (1<<3);
	PORTB ^= (1<<2);						
	_delay_ms(100);
}
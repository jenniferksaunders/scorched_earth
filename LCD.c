/*
 * LCD.c
 *
 * Created: 32/13/2015 12:34:56 AM
 *  Author: Michael
 */ 

#include "LCD.h"
#include <avr/io.h>
#include <util/delay.h>
#include "ASCIIFont.h"	// Font pixel maps

/**
* Function writes a byte to the LCD (uses "bit bashing")
*	- dc: will interpret the byte as a command if dc = 0, as data if dc = 1
*	- data: the byte to be written to the LCD
*/
void LCDWrite(unsigned char dc, unsigned char data) {
	// Set the DC pin based on the parameter 'dc'
	OUTPUT_WRITE(PORTB,DCPIN,dc);
	
	// Pull the SCE/SS pin low to signal to the LCD we have data
	OUTPUT_LOW(PORTD,SCEPIN);
	
	// Write the byte of data using "bit bashing"
	int i;
	for(i = 7; i >= 0; i--) {
		OUTPUT_LOW(PORTF, SCKPIN) ;
		if(data>>i & 1 == 1) {
			OUTPUT_HIGH(PORTB, DINPIN);
		} else {
			OUTPUT_LOW(PORTB, DINPIN);
		}
		OUTPUT_HIGH(PORTF, SCKPIN);
	}
	
	// Pull SCE/SS high to signal the LCD we are done
	OUTPUT_HIGH(PORTD,SCEPIN);
}

/**
* Initialise the LCD with the desired settings. Must be done in every program before using the LCD
*	- contrast: Contrast for the display (use 1 of the 3 constants defined in LCD.h)
*/
void LCDInitialise(unsigned char contrast) {
	// Configure all of the pins correctly
	SET_OUTPUT(DDRD, SCEPIN);
	SET_OUTPUT(DDRB, RSTPIN);
	SET_OUTPUT(DDRB, DCPIN);
	SET_OUTPUT(DDRB, DINPIN);
	SET_OUTPUT(DDRF, SCKPIN);
		
	// Reset the screen
	OUTPUT_LOW(PORTB, RSTPIN);
	OUTPUT_HIGH(PORTD, SCEPIN);
	OUTPUT_HIGH(PORTB, RSTPIN);
  
  	// Send configuration commands to the screen
	LCDWrite(LCD_C, 0x21); 				// Enable LCD extended command set
	LCDWrite(LCD_C, 0x80 | contrast ); 	// Set LCD Vop (Contrast)
	LCDWrite(LCD_C, 0x04);
	LCDWrite(LCD_C, 0x14); 				// LCD bias mode 1:48
  
	LCDWrite(LCD_C, 0x0C); 				// LCD in normal mode.
  	LCDWrite(LCD_C, 0x20); 			//vertical addressing in normal mode	// Enable LCD basic command set
	LCDWrite(LCD_C, 0x0C);

	LCDWrite(LCD_C, 0x40); 				// Reset row to 0
	LCDWrite(LCD_C, 0x80); 				// Reset column to 0
}

/**
* This function clears the LCD by writing 0s to every byte in the LCDs pixel data memory
*/
void LCDClear(void) {
	// Add your code here (Hint: use LCDWrite() function)
	int counter;
	for (counter = 0; counter<(6*84); counter++)
	{
		LCDWrite(LCD_D, 0x00);
	}
}

/**
* This function writes a single character to the screen at the current LCD position
*/
void LCDCharacter(unsigned char character) {
	
	/**
	* Blank pixel column before the character, to increase readability
	* Also, our font is only 5 pixels wide, since our LCD is 84 pixels wide
	* we can fit twelve 7 pixel wide characters across.
	*/
	LCDWrite(LCD_D, 0x00);	//space on left side
	
	/**
	* Add your code here (Hints: Use LCDWrite() function. Look at the contents of ASCIIFont.h!)
	*	To get the first byte of the character from the ASCII table in ASCIIFont.h:
	*	char firstByte = ASCII[character - 0x20][1];
	*/
	 
	int columnCounter;
	for (columnCounter = 0; columnCounter<5; columnCounter++) 	//write middle 5 columns
	{
		LCDWrite(LCD_D, ASCII[character - 0x20][columnCounter]);
	}	

	/**
	* Blank pixel column after the character, to increase readability
	*/
	LCDWrite(LCD_D, 0x00);	//space on right side
}

/**
* This function writes an entire string to the screen
*/
void LCDString(unsigned char *characters) {
	/** 
	* Add your code here 
	*	- Hint 1: use LCDCharacter() function
	*	- Hint 2: you've been given a char array (control structure writing until reach null)
	*/
	
	char c = characters[0];	//get first character
	
	while (!(c == '\0'))	//while not at end of characters
	{	
		LCDCharacter(c);		//write characters to screen
		*characters++;			//increment character pointer
		c = *characters;		//get next character
	}
}

/**
* This function sets the position of LCD cursor
*/
void LCDPosition(unsigned char x, unsigned char y) {
	// Add your code here. (Hint: use LCDWrite() to write a COMMAND)
	
	LCDWrite(LCD_C, (1<<7) + x);
	LCDWrite(LCD_C, (1<<6) + y); 
}



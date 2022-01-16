#include "helper.h"

/**
* Using structures in functions
*/
unsigned char shouldElectronicDevicesBeSwitchedOff(struct Plane p) {
	// Check conditions based on the plane status
	if (p.is_flying && (p.is_taking_off || p.is_landing)) {
		return 1;
	} else {
		return 0;
	}
}

void resetPosition(struct Plane* p) {
	p->latitude = 0.0;		// The '->' operator dereferences and gets the component value
	(*p).longitude = 0.0;	// This is the equivalent of above but does it in two steps (dereference + access component)
}

/**
* Random number generation:
*	- 'seedWithButtonPress()': seeds the random number generator with an iterated count while waiting for a button press
*	- 'randInRange()': uses the random number generator (assumes it has been seeded) to produce a random number between 
*		'min' (inclusive') and 'max' (not inclusive)
*/
void seedWithButtonPress() {
	// Configure port B for the buttons....
	DDRB |= 0b00001100;

	// Wait for the button press on SW0 while iterating the seed
	unsigned int seed = 0;
	while (!((PINB >> PB0) & 1)) {
		seed++;
	}

	// Seed the random number generator
	srand(seed);
	//dbgI((int) seed, 10);
}

int randInRange(int min, int max) {
	return rand()%(max-min) + min; // Modulo to get between range + offset (slight bias based on max of rand() output)
}

/**
* Terrain generation function: 
*	Function fills the 'y_coords' array with a series of points that will outline mountainous terrain on a screen.
*	The uses defines (see TERRAIN_* defines) for height zones based on an assumed height of 48, and assumes a section 
*	width of 4 pixels ('TERRAIN_SECTION_WIDTH'). If you wish to change these assumptions (i.e. screen width),
*	then you will have to create a modified version.
* Input parameters:
*	- 'y_coords': pre-allocated array for y coords
*	- 'length_y_coords': length of the preallocated array 'y_coords'
* Output parameters:
*	None
*/
void generateTerrainPoints(int* y_coords, int length_y_coords) {
	// Helper Variables
	float prevGrad = 0;
	float currGrad = 0;
	int endZoneFlag = 0;
	int ii = 0;
	
	// Get Starting Y Pixel Coordinate
	y_coords[0] = 36;
	
	// Get Remaining Y Pixel Coordinate
	for(ii = 1; ii < length_y_coords; ii++) {
		// Check if prevGrad == 0, else if new y pixel coordinate would be in end zones
		if(prevGrad == 0) {
			// Check to see if in upper or lower zones
			if(y_coords[ii-1] < TERRAIN_UPPER_ZONE) {
				// At top of screen add between 0-7 pixels
				y_coords[ii] = y_coords[ii-1] + randInRange(0,8);
			} else if(y_coords[ii-1] > TERRAIN_LOWER_ZONE) {
				// At bottom of screen subtract between 0-7 pixels
				y_coords[ii] = y_coords[ii-1] - randInRange(0,8);
			} else {
 				// In safe zone add/subtract between (-4)-3 pixels
				y_coords[ii] = y_coords[ii-1] + randInRange(-4,4);
			}
		} else if((y_coords[ii-1]+round(prevGrad*TERRAIN_SECTION_WIDTH)) < TERRAIN_UPPER_ZONE || endZoneFlag == 1) {
			// Calc new grad, bias it so that it adds pixels so levels off and then slopes downwards
			currGrad = prevGrad + fabs((randInRange(75,100)/100.0)*prevGrad);
			y_coords[ii] = round(currGrad*TERRAIN_SECTION_WIDTH) + y_coords[ii-1];
			
			if(endZoneFlag == 1) {
				endZoneFlag = 0;
			} else {
				endZoneFlag = 1;
			}
		} else if((y_coords[ii-1]+round(prevGrad*TERRAIN_SECTION_WIDTH)) > TERRAIN_LOWER_ZONE || endZoneFlag == 2) {
			// Calc new grad, bias it so that it subtracts pixels so levels off and then slopes upwards
			currGrad = prevGrad - fabs((randInRange(75,100)/100.0)*prevGrad);
			y_coords[ii] = round(currGrad*TERRAIN_SECTION_WIDTH) + y_coords[ii-1];
			
			if(endZoneFlag == 2) {
				endZoneFlag = 0;
			} else {
				endZoneFlag = 2;
			}
		} else {
			// Check to see if sloping up or down
			if(prevGrad < 0) {
				// Sloping up. Calc new grad,
				currGrad = prevGrad + (randInRange(-100,20)/100.0)*prevGrad;
				y_coords[ii] = round(currGrad*TERRAIN_SECTION_WIDTH) + y_coords[ii-1];
			} else {
				// Sloping down. Calc new grad,
				currGrad = prevGrad - (randInRange(-20,100)/100.0)*prevGrad;
				y_coords[ii] = round(currGrad*TERRAIN_SECTION_WIDTH) + y_coords[ii-1];
			}
		}
		
		// Update prevGrad
		prevGrad = (y_coords[ii] - y_coords[ii-1])/(float)TERRAIN_SECTION_WIDTH;
	}
}

/**
* Example debugging function. This function writes an integer (expressing it in with 'places' number of digits) to the 
*	screen at the current LCDPosition
*/
void dbgI(int i, unsigned int places) {
	if (i < 0) {
		LCDString("-");
		i *= -1;
	}

	unsigned int ii, iii, iiii;
	int divisor;
	for (ii = places; ii>0; ii--) {
		// power hack for fun....
		divisor = 1;
		for (iiii = ii-1; iiii>0; iiii--) {
			divisor *= 10;
		}
		iii = i / divisor;
		i -= iii*divisor;
		switch(iii) {
			case 0:
				LCDString("0");
				break;
			case 1:
				LCDString("1");
				break;
			case 2:
				LCDString("2");
				break;
			case 3:
				LCDString("3");
				break;
			case 4:
				LCDString("4");
				break;
			case 5:
				LCDString("5");
				break;
			case 6:
				LCDString("6");
				break;
			case 7:
				LCDString("7");
				break;
			case 8:
				LCDString("8");
				break;
			case 9:
				LCDString("9");
				break;
		}
	}
}
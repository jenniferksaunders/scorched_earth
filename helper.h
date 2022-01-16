#include <avr/io.h>
#include <stdlib.h>
#include <math.h>

#include "LCD.h"

/**
* Plane setting defines
*/
#define PLANE_NUM_PILOTS 3

/**
* Terrain generation defines
*/
#define TERRAIN_UPPER_ZONE 20	//defines y pixel coordinate where upper end zone starts 
#define TERRAIN_LOWER_ZONE 39	//defines y pixel coordinate where lower end zone starts
#define TERRAIN_SECTION_WIDTH 4		//defines the pixel width between coordinates

/**
* Struct definition example (this example shows declaring a structure to represent a 'Plane')
*/
struct Plane {
	// Current rotation status
	float roll;
	float pitch;
	float yaw;

	// Current position in the world
	float latitude;
	float longitude;

	// State variables
	unsigned char is_flying;
	unsigned char is_taking_off;
	unsigned char is_landing;

	// Pilot information
	unsigned long pilot_ids[PLANE_NUM_PILOTS];
};

/**
* Structure variable examples
*/
struct Plane plane_q177;	// Declaring a structure variable called 'plane_q177'
struct Plane* plane_q178;	// Declaring a pointer to structure

/**
* Using structures in functions
*/
unsigned char shouldElectronicDevicesBeSwitchedOff(struct Plane p);	// Will check the plane state variables 
void resetPosition(struct Plane* p);	// Passes a pointer to the plane because the contents of plane will be modified

/**
* Random number generation:
*	- 'seedWithButtonPress()': seeds the random number generator with an iterated count while waiting for a button press
*	- 'randInRange()': uses the random number generator (assumes it has been seeded) to produce a random number between 
*		'min' (inclusive') and 'max' (not inclusive)
*/
void seedWithButtonPress();
int randInRange(int min, int max);

/**
* Terrain generation function: 
*	Function fills the 'y_coords' array with a series of points that will outline mountainous terrain on a screen.
*	The uses defines (see TERRAIN_* defines) for height zones based on an assumed height of 48, and assummes a section 
*	width of 4 pixels ('TERRAIN_SECTION_WIDTH'). If you wish to change these assumptions (i.e. screen width),
*	then you will have to create a modified version.
* Input parameters:
*	- 'y_coords': pre-allocated array for y coords
*	- 'length_y_coords': length of the preallocated array 'y_coords'
* Output parameters:
*	None
*/
void generateTerrainPoints(int* y_coords, int length_y_coords);

/**
* Example debugging function. This function writes an integer (expressing it in with 'places' number of digits) to the 
*	screen at the current LCDPosition
*/
void dbgI(int i, unsigned int places);
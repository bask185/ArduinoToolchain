#include <Arduino.h>

#ifndef button_h
#define	button_h

// //#define 
// #define ON 9 // random numbers, RISING and FALLING are already defined in Arduino.h
// #define OFF 10 // REPLACED FOR HIGH AND LOW

class Debounce {
public:
	Debounce(unsigned char _pin);
	unsigned char getState();
	void debounce();
	void debounce( bool );

private:
	unsigned char state;
	unsigned char pin; 
	bool oldSample = false;
	bool statePrev = false;
};
	
#endif
/*
#include "debounceClass.h"

const int pin3 = 3 ;
const int pin4 = 5 ;

Debounce obj1( pin3 ) ; 
Debounce obj2( pin3 ) ;

unsigned long prevTime = 0 ;

void setup()
{
	Serial.begin( 115200 ) ;
	// CONSTRUCTOR OF OBJECTS SETS THE PINMODE TO INPUT_PULLUP
}

void loop() {
	unsigned long currTime = millis() ;

	if( currTime > prevTime ) {
		prevTime += currTime ;

		obj1.debounceInputs();
		obj2.debounceInputs();
	}


	#define printState(x) case x: Serial.println(#x); break

	byte state = obj1.readInput() ;
	Serial.print("state of obj1 = ") ;
	switch ( state ) {
		printState( ON ) ;
		printState( OFF ) ;
		printState( RISING ) ;
		printState( FALLING ) ;
	}

	state = obj2.readInput() ;
	Serial.print("state of obj2 = ") ;
	switch ( state ) {
		printState( ON ) ;
		printState( OFF ) ;
		printState( RISING ) ;
		printState( FALLING ) ;
	}
}

*/
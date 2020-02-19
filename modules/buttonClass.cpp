#include <Arduino.h>
#include "debounceClass.h"

Debounce::Debounce(unsigned char _pin) {
	pinMode(_button, INPUT_PULLUP);
	pin = _pin; }

Debounce::unsigned char debounceInputs() {
	byte retValue = state;

	if(state == RISING) state = RELEASED; // take note I use a pull-up resistor
	if(state == FALLING)state = PRESSED;  // rising or falling may be returned only once

	return retValue; }

Debounce::unsigned char readInput() {
	static bool oldSample = false, statePrev = false;
	bool newSample = digitalRead(pin);

	if(newSample == oldSample) {	// if the same state is detected atleast twice in 20ms...
	
		if(newSample != statePrev) { // if a flank change occured return RISING or FALLING
			statePrev = newSample;

			if(newSample)	return RISING; 
			else			return FALLING; }

		else {						// or if there is no flank change return PRESSED or RELEASED
			if(newSample)	return ON; 
			else			return OFF; } }

	oldSample = newSample; }





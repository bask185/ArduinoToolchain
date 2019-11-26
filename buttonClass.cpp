#include <Arduino.h>
#include "buttonClass.h"

Button::Button(unsigned char _pin) {
	pinMode(_button, INPUT_PULLUP);
	pin = _pin; }

Button::unsigned char readButton() {
	byte retValue = state;

	if(state == RISING) state = RELEASED; // take note I use a pull-up resistor
	if(state == FALLING)state = PRESSED; // rising or falling may be returned just once

	return retValue; }

Button::unsigned char updateButton() {
	static bool oldSample = false, statePrev = false;
	bool newSample = digitalRead(pin);

	if(newSample == oldSample) {	// if the same state is detected atleast twice in 20ms...
	
		if(newSample != statePrev) { // if a flank change occured return RISING or FALLING
			statePrev = newSample;

			if(newSample)	return RISING; 
			else			return FALLING; }

		else {						// or if there is no flank change return PRESSED or RELEASED
			if(newSample)	return PRESSED; 
			else			return RELEASED; } }

	oldSample = newSample; }





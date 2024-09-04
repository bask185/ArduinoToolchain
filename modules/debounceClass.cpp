/*
 * Copyright (C) 2024 Sebastiaan Knippels, Train-Science
 *
 * To the extent possible under law, the person who associated CC0 with this work
 * has waived all copyright and related or neighboring rights to this work.
 *
 * This work is published from: The Netherlands
 *
 * You can copy, modify, distribute and perform the work, even for commercial purposes,
 * all without asking permission. See the full license for details at:
 * https://creativecommons.org/publicdomain/zero/1.0/
 */

#include "debounceClass.h"
Debounce::Debounce(unsigned char _pin)
{
	pin = _pin;
	
	if( _pin == 255 ) return ;
	pinMode(_pin, INPUT_PULLUP); // take note I use a pull-up resistor by default
}

unsigned char Debounce::getState() {
	byte retValue = state;

	if(state == RISING)  state = HIGH; // take note I use a pull-up resistor
	if(state == FALLING) state = LOW;  // rising or falling may be returned only once

	return retValue; }

void Debounce::debounce() {
	bool newSample = digitalRead(pin);

	if(newSample == oldSample) {	// if the same state is detected atleast twice in 20ms...
	
		if(newSample != statePrev) { // if a flank change occured return RISING or FALLING
			statePrev = newSample ;

			if(newSample)	state = RISING; 
			else			state = FALLING;
		}

		else {						// or if there is no flank change return PRESSED or RELEASED
			if(newSample)	state = HIGH; 
			else			state = LOW;
		}
	}

	oldSample = newSample;
}

void Debounce::debounce( uint8 newSample ) {

	if(newSample == oldSample) {	// if the same state is detected atleast twice in 20ms...
	
		if(newSample != statePrev) { // if a flank change occured return RISING or FALLING
			statePrev = newSample ;

			if(newSample)	state = RISING; 
			else			state = FALLING;
		}

		else {						// or if there is no flank change return PRESSED or RELEASED
			if(newSample)	state = HIGH; 
			else			state = LOW;
		}
	}

	oldSample = newSample;
}

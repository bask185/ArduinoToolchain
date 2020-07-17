#include "shortCircuit.h"

uint8_t trackPower;
uint8_t currentPin;
uint8_t *timer;
uint8_t maxCurrent;
uint8_t autoReboot;
uint8_t state;

#define  ON 1
#define OFF 0

void shortCircuitInit(uint8_t _trackPower, uint8_t _currentPin, uint8* _timer, uint32_t _shuntResistance, uint32_t _maxCurrent, uint8_t _autoReboot){
	autoReboot = _autoReboot;
	trackPower = _trackPower;
	currentPin = _currentPin;

	timer = _timer; // pass adress of timer

	maxCurrent = ( 1024 * _maxCurrent * _shuntResistance ) / ( 5 * 1000000 );

// max current is the adc value corresponding with the provided current and resistande
// the calculation:  max adc value = ( 1024 * I * R ) / 5
// we do not work with decimal values, there for we work with milli Amperes and milli Ohms

uint8_t shortCircuit() {
	static uint8_t pulseCounter;
		
	if( digitalRead( trackPower ) ) { 			// only when the track power pin is enabled (if a weistra short circuits, we cannot measure current during the off cycle.
		int sample = analogRead( currentPin );

		if( sample > maxCurrent ) pulseCounter ++;

		if( pulseCounter > 10 ) { 				// if 10 x 5ms = 50ms short circuit is detected, deactivate power to track
			pulseCounter = 0;

			*timer = 5; 							// 5ms brake after ADC sample.

			state = OFF;
			digitalWrite( trackPower, LOW );

			return true;						// function returns true when short circuit. May or may not be used in SW
		}
	} 
	else if( autoReboot == true && state == OFF && (*timer) == 0 ) {
		*timer = 250;

		pulseCounter ++;
		if( pulseCounter == 20 ) { // after 20 * 250ms = 5000ms or 5s the track power may be automaticall re-applied if this is turned on
			pulseCounter = 0;

			state = ON;
			digitalWrite( trackPower, HIGH );
		}
	}

	return false;
}

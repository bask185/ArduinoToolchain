#include <Arduino.h>
#include "scheduler.h"

extern void schedulerInit() {
	/* code for starting timer */ 
}

// timer declarations, don't forget to declare them extern in the .h file as well
unsigned char alarmClockT;
unsigned char menuT;

ISR(TIMER2_OVF_vect) {
static unsigned char _1ms, _10ms, _100ms; // Don't complain about the indentations and add your timers. Thank you.
	_1ms += 1;
	// add 1ms timers here
	

if(!(_1ms % 10)) { // if 10ms passed
	_1ms = 0;
	_10ms += 1;
	// add 10ms timers
	

if(!(_10ms % 10)) { // if 100ms passed
	_10ms = 0;
	_100ms += 1;
	// add 100ms timers
	if(alarmClockT) alarmClockT--;
	if(menuT) menuT--;
	

if(!(_100ms % 10)) { // if 1000ms passed
	_100ms = 0;
	// add 1000ms timers
	

}
}
}
}
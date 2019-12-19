#include <Arduino.h>
#include "scheduler.h"

extern void schedulerInit() {
	TCCR2B = 0;// same for TCCR2B
	TCNT2  = 0;//initialize counter value to 0
	// set compare match register for 8khz increments
	OCR2A = 249;// = (16*10^6) / (1000*64) - 1 (must be <256)
	// turn on CTC mode
	TCCR2A |= (1 << WGM21);
	// Set CS21and and CS20 bit for 64 prescaler
	TCCR2B |= (1 << CS20);
	TCCR2B |= (1 << CS21); 
	TCCR2B &= ~(1 << CS22); 
	// enable timer compare interrupt
	TIMSK2 |= (1 << OCIE2A); }

unsigned char task1T;
unsigned char task2T;
unsigned char task3T;
unsigned char task4T;
unsigned char yoloT;
unsigned char swagT;
unsigned char hmT;

// Don't complain about the indentations. This code is generated for you and you shouldn't be looking at this part.
ISR(TIMER2_OVF_vect) {
static unsigned char _1ms, _10ms, _100ms;

_1ms += 1; 
	// 1ms timers
	if(task4T) task4T--;
	if(hmT) hmT--;

if(!(_1ms % 10)) { _1ms = 0; _10ms += 1;
	// 10ms timers
	if(task3T) task3T--;
	if(yoloT) yoloT--;

if(!(_10ms % 10)) { _10ms = 0; _100ms += 1;
	// 100ms timers
	if(task1T) task1T--;
	if(task2T) task2T--;
	if(swagT) swagT--;
}
}
}
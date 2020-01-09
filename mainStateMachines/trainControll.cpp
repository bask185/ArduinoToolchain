#include <Arduino.h>
#include "trainControll.h"
#include "timers.h"
#include "io.h"

#define entryState if(runOnce) 
#define onState runOnce = false; if(!runOnce)
#define exitState if(!exitFlag) return false; else

static unsigned char state = trainControllIDLE;
static bool enabled, runOnce, exitFlag;

extern void trainControllStop(void) {
	speed = 0;
	digitalWrite(pwmPin, LOW);
	state = trainControllIDLE; }

#define directionPin 2
#define pwmPin 3
#define leftSensor 4
#define rightSensor 5

#define accelerateTime 40
#define decelerateTime 23

extern void trainControllSetState(unsigned char _state) {
	pinMode(directionPin, OUTPUT);
	pinMode(pwmPin, OUTPUT);
	state = _state;
	runOnce = true;
	enabled = true;}

extern unsigned char trainControllGetState(void) {
	return state;}

byte speed = 0;
#define maxSpeed 128

#define State(x) static bool x##F(void)
State(accelerateLeft) {
	entryState {
		digitalWrite(directionPin, LOW); // low
	}
	onState {
		if(!pwmTimer) {
			pwmTimer = accelerateTime; // 20 ms

			if(speed < maxSpeed) speed++;  // about 5 seconds
			analogWrite(pwmPin, speed); }

		if(digitalRead(leftSensor)) exitFlag = true; }
	exitState {
		return true; } }


State(decelerateLeft) {
	entryState {
	}
	onState {
		if(!pwmTimer) {
			pwmTimer = decelerateTime;

			if(speed > 0) speed--;
			analogWrite(pwmPin, speed); }

		if(speed == 0) exitFlag = true; } 
	exitState {
		return true; } }


State(accelerateRight) {
	entryState {
		digitalWrite(directionPin, HIGH); // right
	}
	onState {
		if(!pwmTimer) {
			pwmTimer = accelerateTime; // 40ms
			
			if(speed < maxSpeed) speed++;  // about 5 seconds
			analogWrite(pwmPin, speed); }

		if(digitalRead(rightSensor)) exitFlag = true; }

		exitFlag = true; }
	exitState {
		return true; } }


State(decelerateRight) {
	entryState {
	}
	onState {
		if(!pwmTimer) {
			pwmTimer = decelerateTime;

			analogWrite(pwmPin, speed);
			if(speed > 0) speed--; }	// about 3 seconds

		if(speed == 0) exitFlag = true; } 
	exitState {
		return true; } }

#undef State

#define State(x) break; case x: if(x##F())
extern void trainControll(void) {
	if(enabled) switch(state){
		default: case trainControllIDLE: return;

		State(accelerateLeft) {
			nextState(decelerateLeft, 0); }

		State(decelerateLeft) {
			nextState(accelerateRight, 250); } // 25 second

		State(accelerateRight) {
			nextState(decelerateRight, 0); }

		State(decelerateRight) {
			nextState(accelerateLeft, 250); } // 25 second

		break;}
	else if(!trainControllT) enabled = true; }
#undef State

static void nextState(unsigned char _state, unsigned char _interval) {
	runOnce = true;
	exitFlag = false;
	if(_interval) {
		enabled = false;
		trainControllT = _interval; } 
	state = _state; }

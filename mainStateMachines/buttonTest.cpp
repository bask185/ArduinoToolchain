#include <Arduino.h>
#include "buttonTest.h"
#include "scheduler.h"
#include "io.h"

#define entryState if(runOnce) 
#define onState runOnce = false; if(!runOnce)
#define exitState if(!exitFlag) return false; else

static unsigned char state = buttonTestIDLE;
static bool enabled, runOnce, exitFlag;

extern void buttonTestStop(void) {
	state = buttonTestIDLE; }

extern void buttonTestSetState(unsigned char _state) {
	state = _state;
	runOnce = true;
	enabled = true;}

extern unsigned char buttonTestGetState(void) {
	return state;}



#define State(x) static bool x##F(void)
State(a) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(b) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(c) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(d) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }

#undef State

#define State(x) break; case x: if(x##F())
extern void buttonTest(void) {
	if(enabled) switch(state){
		default: case buttonTestIDLE: return;

		State(a) {
			nextState(b, 0);
			nextState(d, 0);
			nextState(c, 0); }

		State(b) {
			nextState(a, 0);
			nextState(c, 0);
			nextState(d, 0); }

		State(c) {
			nextState(d, 0);
			nextState(b, 0);
			nextState(a, 0); }

		State(d) {
			nextState(a, 0);
			nextState(c, 0);
			nextState(b, 0); }

		break;}
	else if(!buttonTestT) enabled = true; }
#undef State

static void nextState(unsigned char _state, unsigned char _interval) {
	runOnce = true;
	exitFlag = false;
	if(_interval) {
		enabled = false;
		buttonTestT = _interval; } 
	state = _state; }

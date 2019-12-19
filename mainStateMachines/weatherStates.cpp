#include <Arduino.h>
#include "weatherStates.h"
#include "scheduler.h"
#include "io.h"

#define entryState if(runOnce) 
#define onState runOnce = false; if(!runOnce)
#define exitState if(!exitFlag) return false; else

static unsigned char state = weatherStatesIDLE;
static bool enabled, runOnce, exitFlag;

extern void weatherStatesStop(void) {
	state = weatherStatesIDLE; }

extern void weatherStatesSetState(unsigned char _state) {
	randomSeed(analogRead(A0));
	state = _state;
	runOnce = true;
	enabled = true;}

extern unsigned char weatherStatesGetState(void) {
	return state;}



#define State(x) static bool x##F(void)
State(morning) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(afternoon) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(evening) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(night) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(rain) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(thunder) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }

#undef State

#define State(x) break; case x: if(x##F())
extern void weatherStates(void) {
	if(enabled) switch(state){
		default: case weatherStatesIDLE: return;

		State(morning) {
			if(random(0,1))	nextState(afternoon, 0);
			else			nextState(rain, 0); }

		State(afternoon) {
			nextState(evening, 0); }

		State(evening) {
			if(random(0,1))	nextState(night, 0);
			else			nextState(thunder, 0); }

		State(night) {
			nextState(morning, 0); }

		State(rain) {
			nextState(afternoon, 0); }

		State(thunder) {
			nextState(night, 0); }

		break;}
	else if(!weatherStatesT) enabled = true; }
#undef State

static void nextState(unsigned char _state, unsigned char _interval) {
	runOnce = true;
	exitFlag = false;
	if(_interval) {
		enabled = false;
		weatherStatesT = _interval; } 
	state = _state; }

#include <Arduino.h>
#include "railCrossing.h"
#include "scheduler.h"
#include "io.h"

#define entryState if(runOnce) 
#define onState runOnce = false; if(!runOnce)
#define exitState if(!exitFlag) return false; else

static unsigned char state = railCrossingIDLE;
static bool enabled, runOnce, exitFlag;

extern void railCrossingStop(void) {
	state = railCrossingIDLE; }

extern void railCrossingSetState(unsigned char _state) {
	state = _state;
	runOnce = true;
	enabled = true;}

extern unsigned char railCrossingGetState(void) {
	return state;}



#define State(x) static bool x##F(void)
State(closeCrossing) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(openCrossing) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(waitCrossing) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }

#undef State

#define State(x) break; case x: if(x##F())
extern void railCrossing(void) {
	if(enabled) switch(state){
		default: case railCrossingIDLE: return;

		State(closeCrossing) {
			nextState(openCrossing, 0); }

		State(openCrossing) {
			nextState(waitCrossing, 0); }

		State(waitCrossing) {
			nextState(closeCrossing, 0); }

		break;}
	else if(!railCrossingT) enabled = true; }
#undef State

static void nextState(unsigned char _state, unsigned char _interval) {
	runOnce = true;
	exitFlag = false;
	if(_interval) {
		enabled = false;
		railCrossingT = _interval; } 
	state = _state; }

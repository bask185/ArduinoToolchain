#include <Arduino.h>
#include "trainControll.h"
#include "scheduler.h"
#include "io.h"

#define entryState if(runOnce) 
#define onState runOnce = false; if(!runOnce)
#define exitState if(!exitFlag) return false; else

static unsigned char state = trainControllIDLE;
static bool enabled, runOnce, exitFlag;

extern void trainControllStop(void) {
	state = trainControllIDLE; }

extern void trainControllSetState(unsigned char _state) {
	state = _state;
	runOnce = true;
	enabled = true;}

extern unsigned char trainControllGetState(void) {
	return state;}



#define State(x) static bool x##F(void)
State(accelerateLeft) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(deccelerateLeft) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(accelerateRight) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }


State(deccelerateRight) {
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		return true; } }

#undef State

#define State(x) break; case x: if(x##F())
extern void trainControll(void) {
	if(enabled) switch(state){
		default: case trainControllIDLE: return;

		State(accelerateLeft) {
			nextState(deccelerateLeft, 0); }

		State(deccelerateLeft) {
			nextState(accelerateRight, 0); }

		State(accelerateRight) {
			nextState(deccelerateRight, 0); }

		State(deccelerateRight) {
			nextState(accelerateLeft, 0); }

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

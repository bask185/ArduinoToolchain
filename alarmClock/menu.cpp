#include <Arduino.h>
#include "menu.h"
#include "scheduler.h"
#include "io.h"

#define entryState if(runOnce) 
#define onState runOnce = false; if(!runOnce)
#define exitState if(!exitFlag) return false; else

static unsigned char state = menuIDLE;
static bool enabled, runOnce, exitFlag;

extern void menuStop(void) {
	state = menuIDLE; }

extern void menuSetState(unsigned char _state) {
	state = _state;
	runOnce = true;
	enabled = true;}

extern unsigned char menuGetState(void) {
	return state;}



#define State(x) static bool x##F(void)
State(mainMenu) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(auto_) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(test) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(settings) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(automatic) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(inputs) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(outputs) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(testLoadSystems) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(testIndexTable) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(testPunchUnits) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(powerOff) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }

#undef State

#define State(x) break; case x: if(x##F())
extern void menu(void) {
	if(enabled) switch(state){
		default: case menuIDLE: return;

		State(mainMenu) {
			nextState(auto_, 0);
			nextState(test, 0); }

		State(auto_) {
			nextState(settings, 0);
			nextState(inputs, 0);
			nextState(outputs, 0);
			nextState(automatic, 0);
			nextState(mainMenu, 0); }

		State(test) {
			nextState(settings, 0);
			nextState(testLoadSystems, 0);
			nextState(testIndexTable, 0);
			nextState(testPunchUnits, 0);
			nextState(inputs, 0);
			nextState(outputs, 0);
			nextState(mainMenu, 0); }

		State(settings) {
			nextState(test, 0);
			nextState(auto_, 0); }

		State(automatic) {
			nextState(auto_, 0); }

		State(inputs) {
			nextState(test, 0);
			nextState(auto_, 0); }

		State(outputs) {
			nextState(test, 0);
			nextState(auto_, 0); }

		State(testLoadSystems) {
			nextState(test, 0); }

		State(testIndexTable) {
			nextState(test, 0); }

		State(testPunchUnits) {
			nextState(test, 0); }

		State(powerOff) {
			nextState(mainMenu, 0); }

		break;}
	else if(!menuT) enabled = true; }
#undef State

static void nextState(unsigned char _state, unsigned char _interval) {
	runOnce = true;
	exitFlag = false;
	if(_interval) {
		enabled = false;
		menuT = _interval; } 
	state = _state; }

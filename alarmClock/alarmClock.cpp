#include <Arduino.h>
#include "alarmClock.h"
#include "scheduler.h"
#include "io.h"

#define entryState if(runOnce) 
#define onState runOnce = false; if(!runOnce)
#define exitState if(!exitFlag) return false; else

static unsigned char state = alarmClockIDLE;
static bool enabled, runOnce, exitFlag;

extern void alarmClockStop(void) {
	state = alarmClockIDLE; }

extern void alarmClockSetState(unsigned char _state) {
	state = _state;
	runOnce = true;
	enabled = true;}

extern unsigned char alarmClockGetState(void) {
	return state;}



#define State(x) static bool x##F(void)
State(mainMenu) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(setTime) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(setAlarm) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(inputBirthday) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(displayBirthday) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(alarm) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }


State(displayAlarm) {
	entryState{
		
	}
	onState{

		exitFlag = true; }
	exitState {
		return true; } }

#undef State

#define State(x) break; case x: if(x##F())
extern void alarmClock(void) {
	if(enabled) switch(state){
		default: case alarmClockIDLE: return;

		State(mainMenu) {
			nextState(inputBirthday, 0);
			nextState(alarm, 0);
			nextState(setTime, 0);
			nextState(setAlarm, 0);
			nextState(displayAlarm, 0);
			nextState(displayBirthday, 0); }

		State(setTime) {
			nextState(mainMenu, 0); }

		State(setAlarm) {
			nextState(mainMenu, 0); }

		State(inputBirthday) {
			nextState(mainMenu, 0); }

		State(displayBirthday) {
			nextState(mainMenu, 0); }

		State(alarm) {
			nextState(displayBirthday, 0); }

		State(displayAlarm) {
			nextState(mainMenu, 0); }

		break;}
	else if(!alarmClockT) enabled = true; }
#undef State

static void nextState(unsigned char _state, unsigned char _interval) {
	runOnce = true;
	exitFlag = false;
	if(_interval) {
		enabled = false;
		alarmClockT = _interval; } 
	state = _state; }

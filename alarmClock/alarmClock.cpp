#include <Arduino.h>
#include "alarmClock.h"
#include "scheduler.h"
#include "io.h"

#include "keypad.h"
#include "RTC.h"

#define entryState if(runOnce) 
#define onState runOnce = false; if(!runOnce)
#define exitState if(!exitFlag) return false; else

static byte state = alarmClockIDLE;
static bool enabled, runOnce, exitFlag;

extern void alarmClockStop(void) {
	state = alarmClockIDLE; }

extern void alarmClockSetState(byte _state) {
	state = _state;
	runOnce = true;
	enabled = true;}

extern byte alarmClockGetState(void) {
	return state;}

bool ALARM;
char key;
byte cursorX = 0, cursorY = 0;

static void displayTime(byte hT, byte hU, byte mT, byte mU) { 		// hT is hourTens, hU is hourUnits.. etc
	static byte hT_p = 0, hU_p = 0, mT_p = 0, mU_p = 0;				// declare 4 static variables to remember Prev values
	if(hT != hT_p || hU != hU_p || mT != mT_p || mU != mU_p) {		// if any of the figures has changed
		hT_p = ht; hU_p = hU; mT_p = mT; mU_p = mU;					// store the changes

		lcd.setcur(0,0);											// update LCD
		lcd.print(hT); lcd.print(hU); lcd.print(':'); lcd.print(mT); lcd.print(mU); } }

static void displayDate() {
}

static void setCursorX(signed char pos) {
	if(pos == -1) {
		cursorX -= 1;
		if(cursorX == 2) cursorX = 1; // skip position 2
		if(cursorX == 255) cursorX = 0; } // if negative overflow

	else if(pos == 1) {
		cursorX += 1;
		if(cursorX == 2) cursorX = 3; // skip position 2
		if(cursorX > 4) cursorX = 4; }
		
	lcd.setcur(cursorX,cursorY); }

static void makeTime(byte *ptr, byte key, byte min, byte max) {
	
}


#define State(x) static bool x##F(void)
State(mainMenu) {
	entryState {
		lcd.clear();
	}
	onState {
		byte time[4];
		RTC.getTime(&time);
		displayTime(time[0], time[1], time[2], time[3]);

		if(checkAlarm()) {
			ALARM = true;
			exitFlag = true;

		key = keypad.getKey();
		switch(key) {
			case 'B': alarmBit ^= 1;	break;
			case 'C': toggleLamp();		break;
			case '4': addPWM( 1);		break;
			case '7': addPWM(-1);		break;
			
			case '*':
			case '#':
			case 'A':
			case 'D': exitFlag = true;

			default: break; } }
	exitState {
		return true; } }


State(setTime) {
	static byte hourTens = 0, hourUnits = 0, minuteTens = 0, minuteUnits = 0; 
	entryState {
		lcd.clear();
		lcd.setcur(0,1);
		lcd.print("*<># A=Sto,C=bck");
		//		   ________________  16 chars to print
		cursorX = cursorY = 0;
		lcd.setcur(cursorX,cursorY);
		lcd.cursor();
		lcd.blink();
	}
	onState {
		displayTime(hourTens, hourUnits, minuteTens, minuteUnits);

		key = keypad.getKey();
		if(key) switch(cursorX) {
			case 1: makeTime(&hour, key,0,24); break;
			case 4:	makeTime(&minute, key,0,56); break; }

		switch(key) {
			case '*': setCursorX(1); break;
			case '#': setCursorX(4); break;

			case 'A': RTC.storeTime(hourTens, hourUnits, minuteTens, minuteUnits); 
			case 'C':	// both 'A' and 'C' will return but only 'A' will store the set time
			exitFlag = true;

			default: break; } } 
	exitState {
		lcd.noCursor();
		hourTens = hourUnits = minuteTens = minuteUnits = 0; 
		return true; } }


State(setAlarm) {
	static byte hourTens = 0, hourUnits = 0, minuteTens = 0, minuteUnits = 0; 
	entryState {
		lcd.clear();
		lcd.setcur(0,1);
		lcd.print("*<># A=Sto,C=bck");
		//		   ________________  16 chars to print
		cursorX = cursorY = 0;
		lcd.setcur(cursorX, cursorY);
		lcd.cursor();
		lcd.blink();
	}
	onState {
		displayTime(hourTens, hourUnits, minuteTens, minuteUnits);

		key = keypad.getKey();
		if(key >= '0' && key <= '9') {
			switch(cursorX) {
				case 0: makeTime(&hourTens, key,0,2); break;
				case 1: makeTime(&hourUnits, key,0,9); break;
				//case 2: ':'
				case 3: makeTime(&minuteTens, key,0,5); break;
				case 4:	makeTime(&minuteUnits, key,0,9); break; } }

		switch(key) {
			case '*': setCursorX(-1); break;
			case '#': setCursorX( 1); break;

			case 'A': RTC.storeAlarmTime(hourTens, hourUnits, minuteTens, minuteUnits);
			case 'C': // both 'A' and 'C' will return but only 'A' will store the set time
			exitFlag = true;

			default: break; } } 
	exitState {
		return true; } }


State(displayBirthday) {
	entryState {
		lcd.clear(); }
	onState {
		if(showBirthDays()) alarmClockT = 250; // must return 1 when all birthdays are displayed
		if(!alarmClockT()) exitFlag = true; }
	exitState {
		return true; } }


State(alarm) {
	static byte pwm = 0;
	entryState {
		lcd.clear(); }
	onState {
		if(!alarmClockT) {
			alarmClockT = 10;
			setPWM(++pwm);
			if(pwm == 255) pwm = 254;}
		key = keypad.getKey();
		if(key == 'B') exitFlag = true; }
	exitState {
		setPWM(0);
		pwm = 0;
		return true; } }


State(displayAlarm) {
	entryState {
		alarmClockT = 250; // 2.5s
		lcd.clear();
		byte time[4];
		RTC.getAlarmTime(&time);
		displayTime(time[0], time[1], time[2], time[3]); }
	onState {
		if(!alarmClockT) {
			exitFlag = true; } }
	exitState {
		return true; } }

#undef State

#define State(x) break; case x: if(x##F())
extern void alarmClock(void) {
	if(enabled) switch(state){
		default: case alarmClockIDLE: return;

		State(mainMenu) {
			if(ALARM)			nextState(alarm, 0);
			else if(key == '*')	nextState(setTime, 0);
			else if(key == '#')	nextState(setAlarm, 0);
			else if(key == 'D')	nextState(displayAlarm, 0);
			else if(key == 'A')	nextState(displayBirthday, 0); }

		State(setTime) {
			nextState(mainMenu, 0); }

		State(setAlarm) {
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

static void nextState(byte _state, byte _interval) {
	runOnce = true;
	exitFlag = false;
	if(_interval) {
		enabled = false;
		alarmClockT = _interval; } 
	state = _state; }

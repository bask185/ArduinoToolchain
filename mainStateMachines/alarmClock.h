enum alarmClockStates {
	alarmClockIDLE,
	mainMenu,
	setTime,
	setAlarm,
	inputBirthday,
	displayBirthday,
	alarm,
	displayAlarm };

static void nextState(unsigned char, unsigned char);
extern void alarmClock(void); 
extern void alarmClockStop(void);
extern void alarmClockSetState(unsigned char);
extern unsigned char alarmClockGetState(void);

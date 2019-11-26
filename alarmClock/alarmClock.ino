#include "scheduler.h"
#include "roundRobinTasks.h"
#include "alarmClock.h"


#include "menu.h"


void setup() {
	schedulerInit();
	alarmClockSetState(mainmenu);
}

void loop() {
	// readSerialBus();
	processRoundRobinTasks();

	alarmClock();
	menu();
}
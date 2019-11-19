#include "scheduler.h"
#include "roundRobinTasks.h"
#include "alarmClock.h"


#include "menu.h"


void setup() {
	schedulerInit();
}

void loop() {
	// readSerialBus();
	processRoundRobinTasks();

	alarmClock();
	menu();
}
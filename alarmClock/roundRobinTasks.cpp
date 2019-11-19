
#include "roundRobinTasks.h"
#include "io.h"
#define nTasks 2

extern void processRoundRobinTasks(void) {
	static unsigned char taskCounter = 0;
	if(++taskCounter == nTasks) taskCounter = 0;

	switch(taskCounter) {
		case 0:
		/* fill in a task */
		break;

		case 1:
		/* fill in a task */
		break; } }
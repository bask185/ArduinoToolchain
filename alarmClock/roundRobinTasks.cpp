
#include "roundRobinTasks.h"
#include "io.h"


extern void processRoundRobinTasks(void) {
	static unsigned char taskCounter = 0;
	taskCounter++;

	switch(taskCounter) {
		default: taskCounter = 0;

		case 0:
		checkAlarmTime();
		break; 
		
		case 1:
		if(winterTime)	checkSummerTime();
		else			checkwinterTime(); } }
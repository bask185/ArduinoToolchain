# Round Robin tasks
Most programs often have a need for certain functions which are to be called every program cycle. Think of reading the serial bus, polling an IO extender or checking an emergency switch. These functions are better known as round robin tasks. 

Some tasks are more important than others. Therefor I split the RR tasks in to high priority tasks and low priority tasks. The low priority tasks are handled one task at the time. With every program cycle only one low priority task and every high priority task will be performed. This is done in the function `processRoundRobinTasks()` And this function looks default as follows:
```
extern void processRoundRobinTasks(void) {
	static unsigned char taskCounter = 0;
	taskCounter ++;
	
	// HIGH PRIORITY TASKS
	readSerialBus();
	updateIO();  // module for I2C IO extender <- example
	CAN();	// module for CAN bus
	
	
	// LOW PRIORITY TASKS
	switch(taskCounter) {
		default: taskCounter = 0;

		case 0:
		debounceButtons();
		break;
		
		case 1:
		readEstop();	// emergency switch
		break;

		case 2:
		/* fill in a task */
		break; } }
```
It is a switch-case which every cycle increaes it's taskCounter. When the taskCounter reaches a value for which there is no case. The default case will default it to 0. And as there is no break statement case 0 will be immediatly executed. Because of the default case it is not needed to #define the ammount of tasks.

All you need to do to add an task is to simple add a new case and increment the case number. You don't have to be concerned with the taskCounter at all. 

It is important to know that this function is generated in the generateMain.py script (which obviously does more than generating just the main). You can easily modify the script. If every project of your's have the same round robin tasks you can simply add these in the script.
Than you don't have to 'retype' or Ctrl-C -> Ctrl V the same functions over and over again.

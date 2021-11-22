# Round Robin tasks
Most programs often have a need for certain functions which are to be called every program cycle. Think of reading the serial bus, polling an IO extender or checking an emergency switch. These functions are better known as round robin tasks. 

Some tasks are more important than others. Therefor I split the RR tasks in to high priority tasks and low priority tasks. The low priority tasks are handled one task per program cycle. With every program cycle only one low priority task and every high priority task is performed. This is done in the function `processRoundRobinTasks()` And this function looks default as follows (with a little bit filled in):
```c
extern void processRoundRobinTasks(void)
{
    static unsigned char taskCounter = 0;

    // HIGH PRIORITY TASKS
    readSerialBus();    // read serial bus every cycle
    updateIO();         // module for I2C IO extender <- example
    CAN();	            // module for CAN bus


    // LOW PRIORITY TASKS
    switch( ++taskCounter )     // if task counter becomes bigger than the highest task, the default case will reset the counter to 0
    {
    default: 
    case 0:
        taskCounter = 0;

        debounceButtons();
        break;

    case 1:
        readEstop();    // emergency switch
        break;

    case 2:
        /* fill in a task */
        break;
    }
}
```
It is a switch-case which every cycle increaes it's taskCounter. When the taskCounter reaches a value for which there is no case. The default case will default it to 0. And as there is no break statement the default label falls through to case 0 which will be immediatly executed. Because of the default case it is not needed to #define the ammount of tasks. Therefor we cannot forget to do just that.

All you need to do to add a low priority task is to simply add a new case and increment the case number. You don't have to be concerned with the taskCounter at all. 

Take note that this function is generated in the assembleProject.py script. You can easily modify the script yourself if you like. If every project of your's always have the same round robin tasks you can simply add these to the script.
Than you don't have to 'retype' or Ctrl-C -> Ctrl V the same functions over and over again. 

If you have no need for these round robin tasks you can also remove them from your project.

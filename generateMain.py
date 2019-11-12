#!/usr/bin/python

import os

def getStateMachines() : # function tested!
    Files = []

    for root, dirs, fileList in os.walk(".", topdown=False):
        if root == ".\mainStateMachines" :
            Files = fileList
            
    i = 0
    string = []

    for file in Files:
        if i % 2 == 0:
            string.append(file[:-4])
        i += 1
    return string



print("Type name of new project")
folder = input()
try:
    os.makedirs(folder)
except OSError:
    pass

stateMachines = getStateMachines()



with open(folder + "/" + folder + ".ino", "w") as main:             #main.c
    main.write('#include "scheduler.h"\n')
    #main.write('#include " .h"\n') #fill in custom libraries
    #main.write('#include " .h"\n')    
    
    for machine in stateMachines:
        main.write('#include "' + machine + '.h"\n\n\n')
        

    main.write("void setup() {\n")
    main.write("\tschedulerInit();\n")
    main.write("}\n\n")
    main.write("void loop() {\n")
    main.write("\treadSerialBus();\n")
    main.write("\tprocessRoundRobinTasks();\n\n")
    
    for machine in stateMachines:
        main.write("\t" + machine + "();\n")
    main.write("}")



    with open(folder + "/scheduler.c", "w") as scheduler:  #scheduler.c 
        scheduler.write("extern void schedulerInit() {\n\t")
        scheduler.write("/* code for starting timer */ \n}\n\n")

       	for machine in stateMachines:
            scheduler.write("uint8 " + machine + "T;\n")

        scheduler.write("""ISR(TIMER2_OVF_vect) {
	static uint8 _1ms, _10ms, _100ms;
	_ms += 1;
	//	add 1ms timers here

	if(!(_1ms % 10)) { // if 10ms passed
		_1ms = 0;
		_10ms += 1;
		// add 10ms timers

	if(!(_10ms % 10)) { // if 100ms passed
		_10ms = 0;
		_100ms += 1;
		// add 100ms timers

	if(!(_100ms % 10)) { // if 1000ms passed
		_100ms = 0;
		// add 100ms timers


} } }""")
    scheduler.close()




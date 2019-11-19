#!/usr/bin/python

import os
import shutil, errno

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

src = "mainStateMachines"
dest = folder
for src_dir, dirs, files in os.walk(src):
    dst_dir = src_dir.replace(src, dest, 1)
    if not os.path.exists(dst_dir):
        os.makedirs(dst_dir)
    for file_ in files:
        src_file = os.path.join(src_dir, file_)
        dst_file = os.path.join(dst_dir, file_)
        if os.path.exists(dst_file):
            os.remove(dst_file)
        shutil.copy(src_file, dst_dir)

stateMachines = getStateMachines()



with open(folder + "/" + folder + ".ino", "w") as main:             #main.c
    main.write('#include "scheduler.h"\n')
    main.write('#include "roundRobinTasks.h"\n')
    #main.write('#include " .h"\n') #fill in custom libraries
    #main.write('#include " .h"\n')    
    
    for machine in stateMachines:
        main.write('#include "' + machine + '.h"\n\n\n')
        

    main.write("void setup() {\n")
    main.write("\tschedulerInit();\n")
    main.write("}\n\n")
    main.write("void loop() {\n")
    main.write("\t// readSerialBus();\n")
    main.write("\tprocessRoundRobinTasks();\n\n")
    
    for machine in stateMachines:
        main.write("\t" + machine + "();\n")
    main.write("}")



    with open(folder + "/scheduler.cpp", "w") as scheduler:  #scheduler.c 
        scheduler.write("#include <Arduino.h>\n")
        scheduler.write('#include "scheduler.h"\n\n')
        scheduler.write("extern void schedulerInit() {\n\t")
        scheduler.write("/* code for starting timer */ \n}\n\n")
        scheduler.write("// timer declarations, don't forget to declare them extern in the .h file as well\n")
       	for machine in stateMachines:
            scheduler.write("unsigned char " + machine + "T;\n")

        scheduler.write("""
ISR(TIMER2_OVF_vect) {
static unsigned char _1ms, _10ms, _100ms; // Don't complain about the indentations and add your timers. Thank you.
	_1ms += 1;
	// add 1ms timers here
	

if(!(_1ms % 10)) { // if 10ms passed
	_1ms = 0;
	_10ms += 1;
	// add 10ms timers
	

if(!(_10ms % 10)) { // if 100ms passed
	_10ms = 0;
	_100ms += 1;
	// add 100ms timers\n""")
        for machine in stateMachines:
            scheduler.write("\tif(" + machine + "T) " + machine +"T--;\n")
        scheduler.write("""\t\n
if(!(_100ms % 10)) { // if 1000ms passed
	_100ms = 0;
	// add 1000ms timers
	

}
}
}
}""")
    scheduler.close()


with open(folder + "/scheduler.h", "w") as scheduler:  #scheduler.h
    scheduler.write("extern void schedulerInit();\n\n")
    for machine in stateMachines:
        scheduler.write("extern unsigned char " + machine + "T;\n")
    scheduler.close()


with open(folder + "/roundRobinTasks.cpp", "w") as rr:
    rr.write("""
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
		break;""")

    rr.write(" } }")
    rr.close()


with open(folder + "/roundRobinTasks.h", "w") as rr:
    rr.write("void processRoundRobinTasks();\n")
    rr.close()

with open(folder + "/io.h", "w") as rr:
    rr.close()
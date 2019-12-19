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
folder = "../" +input()
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

folder2 = folder[2:]
print(folder2)

with open(folder + "/" + folder2 + ".ino", "w") as main:             #main.c
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
    main.close()


with open(folder + "/roundRobinTasks.cpp", "w") as rr:
    rr.write("""
#include "roundRobinTasks.h"
#include "io.h"

extern void processRoundRobinTasks(void) {
	static unsigned char taskCounter = 0;
	taskCounter ++;

	switch(taskCounter) {
		default: taskCounter = 0;

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
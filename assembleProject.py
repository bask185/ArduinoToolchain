#!/usr/bin/python

import os
import shutil, errno
import platform

def getStateMachines(type) : # function tested!
    Files = []
    if platform.system() == "Windows":
        slash = '\\'
    else:
        slash = '/'

    if type == "main":
        type = "." + slash +"mainStateMachines"
    else:
        type = "." + slash + "nestedStateMachines"

    for root, dirs, fileList in os.walk(".", topdown=False):
        if root == type :
            Files = fileList
            
    string = []
    for file in Files:
        string.append(file[:-8])
    return string


def moveStateMachines(_src, _dest):
    src = _src
    dest = _dest
    for src_dir, dirs, files in os.walk(src):
        #print(dst_dir)
        dst_dir = src_dir.replace(src, dest, 1)
        if not os.path.exists(dst_dir):
            os.makedirs(dst_dir)
        for file_ in files:
            src_file = os.path.join(src_dir, file_)
            dst_file = os.path.join(dst_dir, file_)
            if os.path.exists(dst_file):
                os.remove(dst_file)
            if "graphml" in src_file:
                shutil.copy(src_file, "yEd_stateMachines_repo")
                shutil.move(src_file, dst_dir + "/stateDiagrams") # make me move
            else:
                shutil.move(src_file, dst_dir) # make me move

def assembleTimersTab():
    with open("timers.tab", "w") as timers:
        for machine in stateMachines:
                timers.write(machine + "T\t10\n")
        # if errorHandler == 1:
        #     timers.write("errorT\t2\n")
        # if lightHandler == 1:
        #     timers.write("lightHandlerT\t100\n")
        timers.close()

def pickModules():
    #clear = lambda: os.system('cls') #on Windows System
    #clear()

    modules = os.listdir("./modules/")
    print("Select the modules you want to import\ntype 'done' when you are ready")
    for i, module in enumerate(modules):
        if i % 2 == 0:
            print("{:2d}        {}".format(int(i/2), module[:-4]))

    retValue = ""
    while retValue != 'done':
        retValue = input()
        if retValue != 'done':
            try:
                i = int(retValue) * 2
                print(modules[i] + " picked")
                src = "modules/" + modules[i]
                #print(src)
                dest = folder + "/src/modules"
                #print(dest)
                shutil.copy(src, dest)
                src = "modules/" + modules[i+1]
                shutil.copy(src, dest)
            except:
                print("you entered a wrong value, dipshit. try again!")
    #clear()

def copyAllFiles():
    shutil.copy("updateTimers.py"   , folder)
    shutil.move("timers.tab"        , folder)
    shutil.copy("updateIO.py"       , folder)
    shutil.copy("io.tab"            , folder)
    shutil.copy("serial.cpp"        , folder)
    shutil.copy("serial.h"          , folder)

def assembleMain():
    folder2 = folder[2:]
    with open(folder + "/" + folder2 + ".ino", "w") as main:             #main.c
        main.write('#include "src/basics/timers.h"\n')
        main.write('#include "src/basics/io.h"\n')
        main.write('#include "roundRobinTasks.h"\n')
        #main.write('#include " .h"\n') #fill in custom libraries
        #main.write('#include " .h"\n')    
        
        for machine in stateMachines:
            main.write('#include "' + machine + '.h"\n')
            
        main.write("\nvoid setup() {\n")
        main.write("\tinitTimers();\n")
        main.write("\tinitIO();\n")
        main.write("\tSerial.begin(115200);\n")
        for machine in stateMachines:
            main.write("\t" + machine + "Init();\n")
        main.write("}\n\n")

        main.write("void loop() {\n")
        main.write("\tprocessRoundRobinTasks();\n\n")
        
        for machine in stateMachines:
            main.write("\t" + machine + "();\n")
        main.write("}")
        main.close()

def assembleRoundRobinTasks():
    with open(folder + "/roundRobinTasks.cpp", "w") as rr:
        rr.write("""
#include "roundRobinTasks.h"
#include "src/basics/io.h"

extern void processRoundRobinTasks(void) {
	static unsigned char taskCounter = 0;

// HIGH PRIORITY ROUND ROBIN TASKS
	//readSerialBus();
	//updateIO();

// LOW PRIORITY ROUND ROBIN TASKS
	taskCounter ++;
	switch(taskCounter) {
		default: taskCounter = 0;

		case 0:
		/* fill in a task */
		break;

		case 1:
		/* fill in a task */
		break;
	}
}""")

        rr.close()


    with open(folder + "/roundRobinTasks.h", "w") as rr:
        rr.write("void processRoundRobinTasks();\n")
        rr.write("#define updateIO(); updateOutputs(); \\\n")
        rr.write("updateInputs();")
        rr.close()

def createFolders():
    folder = input("Type name of new project\n")
    folder = "../" + folder
    try:
        print(folder)
        os.makedirs(folder)
        os.makedirs(folder + "/src")
        os.makedirs(folder + "/src/modules")
        os.makedirs(folder + "/src/basics")
        os.makedirs(folder + "/stateDiagrams")
        return folder
    except OSError:
        print("ERROR FOLDER EXISTS")
        pass


### BEGIN SCRIPT ###

folder = createFolders()

stateMachines = getStateMachines("nested") #GENERATE ALL NESTED STATE MACHINES
for machine in stateMachines:
    os.system("python stateMachineGenerator.py " + machine + ".graphml" + " nested")

stateMachines = getStateMachines("main")   #GENERATE ALL MAIN STATE MACHINES
for machine in stateMachines:
    os.system("python stateMachineGenerator.py " + machine + ".graphml" + " main")

moveStateMachines("nestedStateMachines", folder)

moveStateMachines("mainStateMachines", folder)

assembleTimersTab()

pickModules()

copyAllFiles()

assembleMain()

assembleRoundRobinTasks()

os.chdir(folder)
os.system("python updateTimers.py")
os.system("python updateIO.py")

input("press <ENTER> to close the program")

### END SCRIPT ###

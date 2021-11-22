#!/usr/bin/python

import os
import shutil, errno
import platform

projectName = ""

def getStateMachines() : # function tested!
    Files = []
    if platform.system() == "Windows":
        slash = '\\'
    else:
        slash = '/'

    type = "." + slash +"stateMachines"

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
                #shutil.copy(src_file, "yEd_stateMachines_repo")        // no longer copy stuff here, not really needed for this repository
                shutil.move(src_file, dst_dir + "/stateDiagrams") # make me m ove instead of copy
            else:
                shutil.move(src_file, dst_dir)


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
                dest = folder + "/src/"
                #print(dest)
                shutil.copy(src, dest)
                src = "modules/" + modules[i+1]
                shutil.copy(src, dest)
            except:
                print("you entered a wrong value, dipshit. try again!")
    #clear()

def copyAllFiles():
    #shutil.copy("updateTimers.py"   , folder)      // OBSOLETE DUE TO FAVOR OF MILLIS() AND MACROS
    #shutil.move("timers.tab"        , folder)
    shutil.copy("src/updateIO.py"       , folder)
    shutil.copy("src/io.tab"            , folder)
    #shutil.copy("serial.cpp"        , folder)
    #shutil.copy("serial.h"          , folder)
    shutil.copy("src/tasks.json"        , folder +  "/.vscode/" )
    shutil.copy("src/macros.h"          , folder +  "/src/" )
    shutil.copy("src/stateMachineClass.h"   , folder +  "/src/" )
    shutil.copy("src/stateMachineClass.cpp" , folder +  "/src/" )

def assembleMain():
    folder2 = folder[2:]
    with open(folder + "/" + folder2 + ".ino", "w") as main:             #main.c
        #main.write('#include "src/timers.h"\n')
        main.write('#include "src/io.h"\n')
        main.write('#include "roundRobinTasks.h"\n')            # needed?
        #main.write('#include " .h"\n') #fill in custom libraries
        #main.write('#include " .h"\n')    
        
        for machine in stateMachines:
            main.write('#include "' + machine + '.h"\n')
            
        main.write("\nvoid setup()\n{\n")
        main.write("\tinitIO();\n")
        main.write("\tSerial.begin(115200);\n")
        for machine in stateMachines:
            main.write("\t" + machine + "Init();\n")
        main.write("}\n\n")

        main.write("void loop()\n{\n")
        main.write("\tprocessRoundRobinTasks();\n\n")
        
        for machine in stateMachines:
            main.write("\t" + machine + "();\n")
        main.write("}")
        main.close()

def assembleRoundRobinTasks():
    with open(folder + "/roundRobinTasks.cpp", "w") as rr:
        rr.write("""
#include "roundRobinTasks.h"
#include "src/io.h"
#include "src/macros.h"

extern void processRoundRobinTasks(void) 
{
    static unsigned char taskCounter = 0;

// HIGH PRIORITY ROUND ROBIN TASKS


// LOW PRIORITY ROUND ROBIN TASKS
    switch( ++ taskCounter)
    {
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
        rr.close()

def  getProjectName():
    projectName = input("Type name of new project\n")
    return projectName

def createFolders():

    folder = "../" + projectName
    try:
        print(folder)
        os.makedirs(folder)
        os.makedirs(folder + "/src")
        #os.makedirs(folder + "/src/modules") #  OBSOLETE
        #os.makedirs(folder + "/src/basics")
        os.makedirs(folder + "/stateDiagrams")
        os.makedirs(folder + "/.vscode")
        return folder
    except OSError:
        print("ERROR FOLDER EXISTS")
        pass

def assembleBuildScripts():
    with open(folder + "/build.sh", "w") as script:
        script.write("#!/bin/bash\n")
        #script.write("python.exe updateTimers.py\n")
        script.write("python.exe updateIO.py\n")
        script.write("arduino-cli compile -b arduino:avr:nano /c/Users/sknippels/Documents/" + projectName + " -e\n")
        script.write( "exit" )
        script.close()

    with open(folder + "/upload.sh", "w") as script:
        script.write("#!/bin/bash\n")
        #script.write("python.exe updateTimers.py\n")
        script.write("python.exe updateIO.py\n")
        script.write('echo "COMPILING"\n')
        script.write("./build.sh")
        script.write( projectName + "\n" )
        script.write( 'echo "UPLOADING"\n')
        script.write( 'arduino-cli upload -b arduino:avr:nano:cpu=atmega328old -p COM3 -i ~/Documents/software/' + projectName + '/' + projectName + '.arduino.avr.nano.hex\n')
        #script.write( 'rm *.hex *.elf\n') # NOT_NEEDED 
        script.write( "exit" )
        script.close()

    
### BEGIN SCRIPT ###
projectName = getProjectName()

folder = createFolders()

# stateMachines = getStateMachines("nested") #GENERATE ALL NESTED STATE MACHINES       # OBSOLETE 
# for machine in stateMachines:
#     os.system("python stateMachineGenerator.py " + machine + ".graphml" + " nested")

stateMachines = getStateMachines()   #GENERATE ALL MAIN STATE MACHINES
print(stateMachines)
for machine in stateMachines:
    os.system("python ./stateMachineGenerator.py " + machine + ".graphml")

moveStateMachines("stateMachines", folder)

# moveStateMachines("mainStateMachines", folder)    #OBSOLETE
# assembleTimersTab()                               # OBSOLETE

pickModules()

copyAllFiles()

assembleMain()

assembleRoundRobinTasks()

assembleBuildScripts()

os.chdir(folder)                            # change to newly assembled folder, and first run the IO script
#os.system("python updateTimers.py")        # OBSOLETE
os.system("python updateIO.py")

input("press <ENTER> to close the program")

### END SCRIPT ###

#!/usr/bin/env python

import time
import os
import shutil, errno
import platform
import subprocess
import serial.tools.list_ports
from datetime import datetime

projectName = ""
buildDir = ""

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
        if file != ".gitkeep":         # ignore this empty file
            #string.append(file[:-8])    # adds files to list, removes the .graphml extension 
            string.append(file)    # adds files to list, removes the .graphml extension 
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
            #print(src_file)
            dst_file = os.path.join(dst_dir, file_)
            if os.path.exists(dst_file):
                os.remove(dst_file)
            if ".gitkeep" in src_file:
                pass
            elif "graphml" in src_file:
                #shutil.copy(src_file, "yEd_stateMachines_repo")        // no longer copy stuff here, not really needed for this repository
                shutil.move(src_file, dst_dir + "/stateDiagrams") # make me m ove instead of copy
            else:
                shutil.move(src_file, dst_dir)


def pickModules():
    #clear = lambda: os.system('cls') #on Windows System
    #clear()

    # modules = os.listdir("./modules/")
    # print("\n\nSelect the modules you want to import\ntype 'done' when you are ready")
    # for i, module in enumerate(modules):
    #     if i % 2 == 0:
    #         print("{:2d}        {}".format(int(i/2), module[:-4]))

    retValue = ""
    while retValue != 'done':
        modules = os.listdir("./modules/")
        print("\n\nSelect the modules you want to import\ntype 'done' when you are ready")
        for i, module in enumerate(modules):
            if i % 2 == 0:
                print("{:2d}        {}".format(int(i/2), module[:-4]))
        retValue = input()
        if retValue != 'done':
            try:
                i = int(retValue) * 2
                print("\n\n" + modules[i] + " picked")
                src = "modules/" + modules[i]
                #print(src)
                dest = folder + "/src/"
                #print(dest)
                shutil.copy(src, dest)
                src = "modules/" + modules[i+1]
                shutil.copy(src, dest)
                time.sleep(1)
            except:
                print("invalid value!")
    #clear()

def copyAllFiles():
    #shutil.copy("updateTimers.py"   , folder)      // OBSOLETE DUE TO FAVOR OF DIFFERENT TIMING METHOD
    #shutil.move("timers.tab"        , folder)
    shutil.copy("src/updateIO.py"           , folder +  "/src/" )
    shutil.copy("src/io.tab"                , folder)
    #shutil.copy("serial.cpp"        , folder)      // OBSOLETE, will be moved to the module folder
    #shutil.copy("serial.h"          , folder)
    shutil.copy("src/tasks.json"            , folder +  "/.vscode/" )
    shutil.copy("src/macros.h"              , folder +  "/src/" )
    shutil.copy("src/stateMachineClass.h"   , folder +  "/src/" )
    shutil.copy("src/stateMachineClass.cpp" , folder +  "/src/" )
    shutil.copy("src/addDate.py"            , folder +  "/src/" )
    shutil.copy("src/gitInit.sh"            , folder +  "/src/" )
    shutil.copy("src/release.py"            , folder +  "/src/" )
    shutil.copy("src/version.h"             , folder +  "/src/" )

def assembleMain():
    folder2 = folder[2:]
    with open(folder + "/" + folder2 + ".ino", "w") as main:             #main.c
        #main.write('#include "src/timers.h"\n')
        main.write('#include "src/io.h"\n')
        main.write('#include "src/date.h"\n')
        main.write('#include "src/version.h"\n')
        main.write('#include "src/macros.h"\n')
        main.write('#include "roundRobinTasks.h"\n')            # needed?
        #main.write('#include " .h"\n') #fill in custom libraries
        #main.write('#include " .h"\n')    
        
        for machine in stateMachines:
            main.write('#include "' + machine + '.h"\n\n')
            
        main.write("void setup()\n")
        main.write("{\n")
        main.write("    initIO() ;\n")
        main.write("    Serial.begin( 115200 ) ;\n")
        main.write("    Serial.println( version ) ;\n")
        main.write("    Serial.println( date ) ;\n")
        for machine in stateMachines:
            main.write("    " + machine + "Init() ;\n")
        main.write("}\n\n")

        main.write("void loop()\n{\n")
        main.write("    processRoundRobinTasks() ;\n\n")
        
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
    default: 
        taskCounter = 0;
        /* fill in a task */
        break;

    case 1:
        /* fill in a task */
        break;

    case 2:
        /* fill in a task */
        break;
    }
}""")

        rr.close()


    with open(folder + "/roundRobinTasks.h", "w") as rr:
        rr.write("#include <Arduino.h>\n\n")
        rr.write("void processRoundRobinTasks() ;\n")
        rr.close()

def getProjectName():

    projectName = input("Type name of new project\n")
    return projectName

def getBoardType():
    
    subprocess.call( "arduino-cli.exe board listall" )
    #subprocess.call( ['sh', './listAllBoards.sh'])
    print("For what board will you be compiling?")
    boardName = input("Choose any board name behind arduino:avr:\n")
    return "arduino:avr:" + boardName 
    #boardType =  input("for what board will you be compiling\n")

def createFolders():
    path = os.getcwd()

    print("Current Directory: " + path)
    buildDir = os.path.abspath(os.path.join(path, os.pardir))
    #print( buildDir )

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
        exit()
        pass

def assembleBuildScripts():
    with open(folder + "/src/build.py", "w") as script:
        script.write("#!/usr/bin/env python\n")
        script.write('import os\n')
        script.write('os.system("python src/updateIO.py")\n')
        script.write('os.system("python src/addDate.py")\n')
        script.write("print('BUILDING')\n")
        script.write("os.system('arduino-cli compile -b " + FQBN + buildDir + " -e')\n") #FIXME THIS PATH NEED FIXING WITH CURRENT FOLDER    + projectName + 
        script.write( "exit" )
        script.close()

    ports = serial.tools.list_ports.comports()

    for port, desc, hwid in sorted(ports):
        lastPort = port

    with open(folder + "/src/upload.py", "w") as script:
        script.write("#!/usr/bin/env python\n")
        script.write('import os\n')
        script.write('os.system("python src/build.py")\n')

        script.write( 'print("UPLOADING")\n')
        script.write('os.system("arduino-cli upload -b ' + FQBN + ' -p ' + lastPort + ' -i ' + buildDir + './build/')
        for letter in FQBN:
            if( letter == ':'):
                script.write('.')
            else:
                script.write( letter )
        script.write('/' + projectName + '.ino.hex")\n') #FIXME THIS PATH NEED FIXING
  

        #script.write( 'rm *.hex *.elf\n') # NOT_NEEDED 
        script.write( "exit" )
        script.close()

    dateTimeObj = datetime.now()
    with open(folder + "/changelog.txt", "w") as log:   # makes changelog
        log.write(projectName + "\nv0.0.0   Date/time:" + str(dateTimeObj) + "\nProject assembled\n\n" )
        log.close()

    
### BEGIN SCRIPT ###
projectName = getProjectName()

FQBN = getBoardType()
print (FQBN + " selected\n" )
time.sleep( 1 )

folder = createFolders()

stateMachines = getStateMachines()   #GENERATE ALL MAIN STATE MACHINES
print(stateMachines)
for machine in stateMachines:
    os.system("python ./src/stateMachineGenerator.py " + "stateMachines/"+ machine )

moveStateMachines("stateMachines", folder)

pickModules()

copyAllFiles()

assembleMain()

assembleRoundRobinTasks()

assembleBuildScripts()

input("press <ENTER> to close the program")

### END SCRIPT ###

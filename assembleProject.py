#!/usr/bin/env python


# TODO
# prompt if roundRobintasks are desired?            DONE
# added more macros for serial communication        WAS DONE ALREADY
# change FQBN string so other than arduino:avr board may also be used DONE
# investigate if following can be done:
# change method of generating state machines so that assembleProject.py can be used as global executable.
# Recommended to prompt if state machines are desired
# or split the state machine script all togather so that it can be used anywhere locally

import time
import glob
import os
import shutil, errno
import platform
import subprocess
import serial.tools.list_ports
from datetime import datetime


projectName = ""
buildDir = ""
roundRobinTasks = 0

def clear():
    time.sleep(1)
    os.system('cls')

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
            #print("moving " + src_file)
            dst_file = os.path.join(dst_dir, file_)
            if os.path.exists(dst_file):
                os.remove(dst_file)
            if ".gitkeep" in src_file:
                pass
            elif "graphml" in src_file:
                #shutil.copy(src_file, dst_dir + "/stateDiagrams")        #no longer copy stuff here, not really needed for this repository
                shutil.move(src_file, dst_dir + "/stateDiagrams") # make me m ove instead of copy
            else:
                shutil.move(src_file, dst_dir)


def pickModules():

    # modules = os.listdir("./modules/")
    # print("\n\nSelect the modules you want to import\ntype 'done' when you are ready")
    # for i, module in enumerate(modules):
    #     if i % 2 == 0:
    #         print("{:2d}        {}".format(int(i/2), module[:-4]))

    retValue = ""
    while retValue != 'done':
        modules = os.listdir("./modules/")
        print("Select the modules you want to import\ntype 'done' when you are ready")
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
    #shutil.copy("src/updateIO.py"           , folder +  "/src/" ) # replaced by global executable
    shutil.copy("src/io.tab"                , folder)
    #shutil.copy("serial.cpp"        , folder)      // OBSOLETE, may be moved to the module folder if still used...
    #shutil.copy("serial.h"          , folder)
    shutil.copy("src/tasks.json"            , folder +  "/.vscode/" )
    # shutil.copy("src/stateMachine.code-snippets", folder +  "/.vscode/" )  # REDUNDANT one should use a global code-snippet instead, much better
    shutil.copy("src/macros.h"              , folder +  "/src/" )
    shutil.copy("src/stateMachineClass.h"   , folder +  "/src/" )
    shutil.copy("src/stateMachineClass.cpp" , folder +  "/src/" )
    #shutil.copy("src/addDate.py"            , folder +  "/src/" )  # replaced by global executable
    #shutil.copy("src/gitInit.py"            , folder +  "/src/" )  # replaced by global executable
    #shutil.copy("src/release.py"            , folder +  "/src/" )  # replaced by global executable
    shutil.copy("src/version.h"             , folder +  "/src/" )
    #shutil.copy("src/changelog.py"             , folder +  "/src/" )

def assembleMain():
    folder2 = folder[2:]
    with open(folder + "/" + folder2 + ".ino", "w") as main:             #main.c
        #main.write('#include "src/timers.h"\n')
        main.write('#include "src/io.h"\n')
        main.write('#include "src/date.h"\n')
        main.write('#include "src/version.h"\n')
        main.write('#include "src/macros.h"\n')
        if roundRobinTasks == 1 :
            main.write('#include "roundRobinTasks.h"\n')            # needed?
        #main.write('#include " .h"\n') #fill in custom libraries
        #main.write('#include " .h"\n')    
        
        for machine in stateMachines:
            machine = machine[:-8]
            main.write('#include "' + machine + '.h"\n\n')
            
        main.write("void setup()\n")
        main.write("{\n")
        main.write("    initIO() ;\n")
        main.write("    Serial.begin( 115200 ) ;\n")
        main.write("    Serial.println( version ) ;\n")
        main.write("    Serial.println( date ) ;\n")
        for machine in stateMachines:
            machine = machine[:-8]
            main.write("    " + machine + "Init() ;\n")
        main.write("}\n\n")

        main.write("void loop()\n{\n")
        if roundRobinTasks == 1 :
            main.write("    processRoundRobinTasks() ;\n\n")
        
        for machine in stateMachines:
            machine = machine[:-8]
            main.write("\t" + machine + "();\n")
        main.write("}")
        main.close()

def assembleRoundRobinTasks():
    answer = input("\nDo you wish to include files for round robin task? [Y/n]\n")
    if answer == 'Y' or answer == 'y':
        print("GENERATING ROUND ROBIN FILES\n")
        time.sleep( 1 )
        roundRobinTasks = 1 

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
    boardName = input("Fill in a FQBN of above list\n")
    return boardName 
    #boardType =  input("for what board will you be compiling\n")

def createFolders():
    path = os.getcwd()

    #print("Current Directory: " + path)
    buildDir = os.path.abspath(os.path.join(path, os.pardir))
    #print( buildDir )

    folder = "../" + projectName
    try:
        #print(folder)
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
    CPU = ""
    CLOCK_FREQ = ""
    CLOCK_SOURCE = ""
    PROGRAMMER = ""

    with open(folder + "/src/build.py", "w") as script:
        if 'nano' in FQBN :
            answer = input("NANO SELECTED, DO YOU USE THE OLD BOOTLOADER? [Y/n]\n\n")
            if answer == 'Y' or answer == 'y':
               CPU = ":cpu=atmega328old" 

        if 'ATTinyCore' in FQBN :
            print("ATTINY CORE SELECTED\n")
            CPU = ':chip=' + input("ENTER CHIP NUMBER \n1634\n2313/4313\n24/44/84\n441/841\n25/45/85\n261/461/861\n87/167\n48/88\n43\n828\n\n")
            time.sleep(1)
            clear()
            CLOCK_FREQ = ",clock=" + input("ENTER CLOCK FREQUENCE IN MHz: 1,4,6,8,12,16,20\n\n")
            time.sleep(1)
            clear()
            CLOCK_SOURCE = input("ENTER CLOCK SOURCE: 'internal' OR 'external'?\n\n")
            time.sleep(1)
            clear()
            PROGRAMMER = " --programmer " + input("""WHAT PROGRAMMER WILL YOU BE USING?\n
usbasp                              USBasp (ATTinyCore)
micronucleusprog                    Micronucleus
parallel                            Parallel Programmer
arduinoasisp                        Arduino as ISP
diamexusbisp                        Diamex USB ISP
dragon                              AVR Dragon ISP mode (ATTinyCore)
usbtinyisp                          USBtinyISP (ATTinyCore) SLOW, for new or 1 MHz parts
ponyser                             Ponyser Programmer
stk500                              Atmel STK500
arduinoasisp32u4                    Arduino Leo/Micro as ISP (ATmega32U4)
usbtinyisp2                         USBtinyISP (ATTinyCore) FAST, for parts running >=2 MHz
atmel_ice                           Atmel-ICE
avrispmkii                          AVRISP mkII
avrisp\n\n
""")
            
        script.write( "#!/usr/bin/env python\n")
        script.write( 'import os\n')
        script.write('import sys\n')
        script.write( "print('ASSEMBLING IO FILES')\n")
        script.write( 'os.system("updateIO.py")\n')
        script.write( "print('ADDING TIME STAMP')\n")
        script.write( 'os.system("addDate.py")\n')
        script.write( "print('BUILDING PROJECT')\n")
        script.write( "x = os.system('arduino-cli compile -b " + FQBN + CPU + CLOCK_FREQ + CLOCK_SOURCE + buildDir + " -e')\n") 
        script.write( "if x == 1 :\n" )
        script.write( "    print('BUILD FAILED!!!')\n" )
        script.write( "    sys.exit(1)\n" )
        script.write( "else :\n" )
        script.write( "    print('BUILD SUCCES!!!')\n" )
        script.write( "    sys.exit(0)\n" )
        script.close()

    ports = serial.tools.list_ports.comports()

    for port, desc, hwid in sorted(ports):
        lastPort = port

    with open(folder + "/src/upload.py", "w") as script:
        script.write("#!/usr/bin/env python\n")
        script.write('import os\n')
        script.write('import sys\n')
        script.write('import serial.tools.list_ports\n\n')
        script.write("""
def get_com_port():
    ports = list(serial.tools.list_ports.comports())
    for port in ports:
        if 'Arduino' in port.description or 'CH340' in port.description:
            return port.device
    return None
\n""")
        script.write("""
com_port = get_com_port()
if com_port is None:
    print("No device on COM port found")
    sys.exit(1)
\n""")
        
        script.write('retCode = os.system("python src/build.py")\n')
        script.write('if retCode == 0 :\n')
        script.write('    print("UPLOADING")\n')


        script.write('    retCode = os.system(f"arduino-cli upload -b ' + FQBN + CPU + CLOCK_FREQ + CLOCK_SOURCE + PROGRAMMER + ' -p ' + "{com_port}" + ' -i ' + buildDir + './build/')
        for letter in FQBN:
            if( letter == ':'):
                script.write('.')
            else:
                script.write( letter )
        script.write('/' + projectName + '.ino.hex")\n')

        script.write('    if retCode == 1 :\n')
        script.write('        print("UPLOADING FAILED!!! ")\n')
        script.write('    else :\n')
        script.write('        print("UPLOADING SUCCES!!! ")\n')
        script.close()        

    dateTimeObj = datetime.now()
    with open(folder + "/changelog.txt", "w") as log:   # makes changelog
        log.write(projectName + "\nv0.0.0   Date/time:" + str(dateTimeObj) + "\nProject assembled\n\n" )
        log.close()

    
### BEGIN SCRIPT ###
clear()
projectName = getProjectName()
clear()

FQBN = getBoardType()
print (FQBN + " selected\n" )
clear()

folder = createFolders()

stateMachines = getStateMachines()   #GENERATE ALL MAIN STATE MACHINES
#print("stateMachines: ")
#print( stateMachines)
for machine in stateMachines:
    print("GENERATING STATE MACHINES")
    print( machine )
    os.system("python ./src/stateMachineGenerator.py " + "stateMachines/"+ machine )

    for file in glob.glob('*.cpp'):
        shutil.move(file, "stateMachines/")
    for file in glob.glob('*.h'):
        shutil.move(file, "stateMachines/")

    #os.system("mv.exe *.cpp stateMachines/") // does not work outside vs code due to lack of git bash
    #os.system("mv.exe *.h stateMachines/")

moveStateMachines("stateMachines", folder)

pickModules()
clear()

copyAllFiles()

assembleRoundRobinTasks()
clear()

assembleMain()

assembleBuildScripts()
clear()

input("FINISHED press <ENTER> to close the program")

### END SCRIPT ###

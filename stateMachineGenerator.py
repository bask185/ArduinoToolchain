#!/usr/bin/python 
import sys
import xml
import os
import platform 

def getStateMachines() : # function tested!
    slash = ""
    Files = []
    print(platform.system())
    if platform.system() == "Windows":
        slash = '\\'
    else:
        slash = '/'
    for root, dirs, fileList in os.walk(".", topdown=False):
        if root == "." + slash + "yEd_stateMachines" :
            Files = fileList

   
    i = 0
    string = []

    for file in Files:
        #if i % 2 == 0:
        string.append(file)
        #print(file)
        i += 1
    return string

stateDiagrams = getStateMachines()

arrowsOut1 = []
arrowsIn1 = []
arrowsOut = []
arrowsIn = []
states1 = []
states = []

file_name = sys.argv[1]
smType = sys.argv[2] # 'c' or 'assembly'

if smType == "main":
    file_name = "mainStateMachines/" + file_name

if smType == "nested":
    file_name = "nestedStateMachines/" + file_name

with open(file_name, "r") as f:
    data = f.readlines()

for line in data: #states
    words = line.split('"')
    if words.count(' autoSizePolicy=') != 0:
        states1.append(words[36])

for arrow in data: # arrows
    words = arrow.split('"')
    if words.count('    <edge id=') != 0:
        arrowsOut1.append(words[3][1:])
        arrowsIn1.append(words[5][1:])
        
for state in states1:
    state = state.split('<')
    states.append(state[0][1:])

f.close()



new_file_name = file_name[:-8]
folder = new_file_name
new_file_name = new_file_name.split('/')
new_file_name = new_file_name[1]
#print(new_file_name)


if smType == "main":
    folder = "mainStateMachines/"
else:
    folder = "nestedStateMachines/"
#        folder + 
with open(folder + new_file_name + ".cpp", "w") as c:
    c.write("// HEADER FILES\n")
    c.write('#include <Arduino.h>\n')
    c.write('#include "' + new_file_name + '.h"\n')
    c.write('#include "serial.h"\n')
    c.write('#include "src/basics/timers.h"\n')
    c.write('#include "src/basics/io.h"\n\n')

    c.write("// MACROS\n")
    c.write("#define stateFunction(x) static bool x##F(void)\n")
    c.write("#define entryState if(runOnce) \n")
    c.write("#define onState runOnce = false; if(!runOnce)\n")
    c.write("#define exitState if(!exitFlag) return false; else\n")
    c.write("#define State(x) break; case x: if(runOnce) Serial.println(#x); if(x##F())\n")
    c.write("#define STATE_MACHINE_BEGIN if(!enabled) { \\\n")
    c.write("    if(!"+ new_file_name +"T) enabled = true; } \\\n")
    c.write("else switch(state){\\\n")
    c.write('    default: Serial.println("unknown state executed, state is idle now"); state = ' + new_file_name + 'IDLE; case ' + new_file_name + 'IDLE: return true;\n')
    c.write("#define STATE_MACHINE_END break;}return false;\n\n\n")

    if smType == "main":
        c.write("//#define beginState\n")
        c.write("#ifndef beginState\n")
        c.write("#error beginState not yet defined\n")
        c.write("#endif\n\n")
    else:
        c.write("#define beginState " + new_file_name + "IDLE\n\n")
        c.write("//#define " + new_file_name + "T\n")
        c.write("#ifndef " + new_file_name + "T\n")
        c.write("#error " + new_file_name + "T is not defined, define this as the parent's timer\n")
        c.write("#endif\n\n") 

    c.write("// VARIABLES\n")
    c.write("static unsigned char state = beginState;\n")
    c.write("static bool enabled = true, runOnce = true, exitFlag = false;\n\n")

    c.write("// FUNCTIONS\n")
    c.write("extern void " + new_file_name + "Init(void)\n")
    c.write("{\n")
    c.write("    state = beginState ;\n")
    c.write("}\n\n")
    
    c.write("extern byte " + new_file_name + "GetState(void)\n")
    c.write("{\n")
    c.write("    return state ;\n")
    c.write("}\n\n")
    
    c.write("extern void " + new_file_name + "SetState(unsigned char _state)\n")
    c.write("{\n")
    c.write("    state = _state ;\n")
    c.write("    runOnce = true ;\n")
    c.write("}\n\n")
    
    c.write("static void nextState(unsigned char _state, unsigned char _interval)\n")
    c.write("{\n")
    c.write("    runOnce = true ;\n")
    c.write("    exitFlag = false ;\n")
    c.write("    if(_interval)\n")
    c.write("    {\n")
    c.write("        enabled = false;\n")
    c.write("        " + new_file_name + "T = _interval ;\n")
    c.write("    }\n")
    c.write("    state = _state ;\n")
    c.write("}\n\n")

    c.write("// STATE FUNCTIONS")
    for state in states:    # print all state functions
        c.write("\nstateFunction(" + state + ") {\n")
        c.write("    entryState {\n        \n    ")
        c.write("}\n    onState {\n\n        exitFlag = true;\n")
        c.write("    }\n")
        c.write("    exitState {\n\n")
        c.write("        return true;\n")
        c.write("    }\n")
        c.write("}\n")

    c.write('\n// STATE MACHINE\n')
    c.write("extern bool "+ new_file_name +"(void) {\n")
    c.write("    STATE_MACHINE_BEGIN\n\n")
    i = -1
    for state in states:
        i = i + 1
        c.write("    State(" + state + ") {")
        nArrows = 0
        for j in range(0, len(arrowsOut1)):
            if(int(arrowsOut1[j]) == i):
                nArrows += 1
                c.write("\n        nextState(" + states[int(arrowsIn1[j])] + ", 0);")
        if nArrows == 0:
            c.write("\n        nextState(" + new_file_name + "IDLE, 0);")
        c.write(" }\n\n")
    c.write("    STATE_MACHINE_END\n")
    # if smType == "nested":
    #     c.write("\n    return false; }\n")
    # else:
    c.write("}\n")


with open(folder + new_file_name + ".h", "w") as h:
    h.write("enum " + new_file_name + "States {")
    h.write("\n    " + new_file_name + "IDLE")
    for state in states:
        h.write(",\n    "+ state)
    h.write(" };\n\n")
    
    h.write("extern bool " + new_file_name + "(void); \n")
    h.write("extern void " + new_file_name + "Init();\n")
    h.write("extern void " + new_file_name + "SetState(unsigned char);\n")
    h.write("extern unsigned char " + new_file_name + "GetState(void);\n") # get currentState
    h.close()
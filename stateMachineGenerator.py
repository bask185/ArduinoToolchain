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
    c.write('#include "src/basics/macros.h"\n')
    c.write('#include "src/basics/io.h"\n\n')

    if smType == "main":
        c.write("//#define beginState\n")
        c.write("#ifndef beginState\n")
        c.write("#error beginState not yet defined\n")
        c.write("#endif\n\n")
    else:
        c.write("#define beginState " + new_file_name + "IDLE\n\n")

    c.write("// VARIABLES\n\n")

    c.write("// FUNCTIONS\n")
    c.write("extern void " + new_file_name + "Init(void)\n")
    c.write("{\n")
    c.write("    sm.nextState( beginState, 0 ) ;\n")
    c.write("}\n\n")
    

    c.write("// STATE FUNCTIONS")
    for state in states:    # print all state functions
        c.write("stateFunction( " + state + " )\n")
        c.write("{\n")
        c.write("    if( sm.entryState() )\n")
        c.write("    {\n")
        c.write("        \n")
        c.write("    }\n")
        c.write("    if( sm.onState() )\n")
        c.write("    {\n")
        c.write("        \n")
        c.write("        sm.exit() ;\n")
        c.write("    }\n")
        c.write("    if( sm.exitState() )\n")
        c.write("    {\n")
        c.write("        \n")
        c.write("        return sm.stateEnd() ;\n")
        c.write("    }\n")
        c.write("}\n\n")

    c.write('\n// STATE MACHINE\n')
    c.write("extern bool "+ new_file_name +"()\n")
    c.write("{\n")
    c.write("    STATE_MACHINE_BEGIN\n\n")
    i = -1
    for state in states:
        i = i + 1
        c.write("    State(" + state + ") {")
        nArrows = 0
        for j in range(0, len(arrowsOut1)):
            if(int(arrowsOut1[j]) == i):
                nArrows += 1
                c.write("\n        sm.nextState( " + states[int(arrowsIn1[j])] + ", 0 ) ;")
        if nArrows == 0:
            c.write("\n        sm.nextState( " + new_file_name + "IDLE, 0 ) ;")
        c.write(" }\n\n")
    c.write("    STATE_MACHINE_END\n")
    c.write("}\n")


with open(folder + new_file_name + ".h", "w") as h:
    h.write("enum " + new_file_name + "States {")
    h.write("\n    " + new_file_name + "IDLE")
    for state in states:
        h.write(",\n    "+ state)
    h.write(" };\n\n")
    
    h.write("extern bool " + new_file_name + "(void); \n")
    h.close()
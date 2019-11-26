#!/usr/bin/python 
import sys
import xml
import os

def getStateMachines() : # function tested!
    Files = []

    for root, dirs, fileList in os.walk(".", topdown=False):
        if root == ".\yEd_stateMachines" :
            Files = fileList
            
    i = 0
    string = []

    for file in Files:
        if i % 2 == 0:
            string.append(file)
        i += 1
    return string

stateDiagrams = getStateMachines()

for index, diagram in enumerate(stateDiagrams):
    print("{} = {}".format(index, diagram))
retValue = input("select state machine\n")
file_name = stateDiagrams[int(retValue)]



arrowsOut1 = []
arrowsIn1 = []
arrowsOut = []
arrowsIn = []
states1 = []
states = []

#file_name = sys.argv[1]
#language = sys.argv[2] # 'c' or 'assembly'


smType = "Is this state machine a 'main' or 'nested' type state machine?"
print(smType)
while smType != "main" and smType != "nested":
    smType = input()
print(smType + " type selected")

with open("yEd_stateMachines/" + file_name, "r") as f:
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
    
#for arrowOut in arrowsOut1:
#    print(arrowOut)

#print("\r\n")

#for arrowIn in arrowsIn1:
    #print(arrowIn)

f.close()


new_file_name = file_name[:-8]


if smType == "main":
    folder = "mainStateMachines/"
else:
    folder = "nestedStateMachines/"
#        folder + 
with open(folder + new_file_name + ".cpp", "w") as c:
    c.write('#include <Arduino.h>\n')
    c.write('#include "' + new_file_name + '.h"\n')
    c.write('#include "scheduler.h"\n')
    c.write('#include "io.h"\n\n')

    c.write("#define entryState if(runOnce) \n")
    c.write("#define onState runOnce = false; if(!runOnce)\n")
    c.write("#define exitState if(!exitFlag) return false; else\n\n")
    
    c.write("static unsigned char state = " + new_file_name + "IDLE;\n")
    c.write("static bool enabled, runOnce, exitFlag;\n\n")
            
    c.write("extern void " + new_file_name + "Stop(void) {\n")     # stop
    c.write("\tstate = " + new_file_name + "IDLE; }\n\n")
            
    c.write("extern void " + new_file_name + "SetState(unsigned char _state) {\n") # get currentState
    c.write("\tstate = _state;\n")
    c.write("\trunOnce = true;\n")
    c.write("\tenabled = true;}\n\n")

    c.write("extern unsigned char " + new_file_name + "GetState(void) {\n") # get currentState
    c.write("\treturn state;}\n\n\n\n")

    
    c.write("#define State(x) static bool x##F(void)")
    for state in states:    # print all state functions
        c.write("\nState(" + state + ") {\n") 
        c.write("\tentryState {\n\t\t\n\t")
        c.write("}\n\tonState {\n\n\t\texitFlag = true; }\n")
        c.write("\texitState {\n\t")
        c.write("\treturn true; } }\n\n")
        
    c.write("#undef State\n\n")
    c.write("#define State(x) break; case x: if(x##F())")    
    if smType == "nested":
        c.write("\nextern bool " + new_file_name + "(void) {\n")
    else:
        c.write("\nextern void " + new_file_name + "(void) {\n")
    c.write("\tif(enabled) switch(state){\n")
    c.write("\t\tdefault: case " + new_file_name + "IDLE: return")
    if smType == "nested": 
        c.write("true;\n\n")
    else:
        c.write(";\n\n")
    i = -1
    for state in states:
        i = i + 1
        c.write("\t\tState(" + state + ") {")
        nArrows = 0
        for j in range(0, len(arrowsOut1)):
            if(int(arrowsOut1[j]) == i):
                nArrows += 1
                c.write("\n\t\t\tnextState(" + states[int(arrowsIn1[j])] + ", 0);")
        if nArrows == 0:
            c.write("\n\t\t\tnextState(" + new_file_name + "IDLE, 0);")
        c.write(" }\n\n")
    c.write("\t\tbreak;}\n")
    c.write("\telse if(!" + new_file_name + "T) enabled = true;")
    if smType == "nested":
        c.write("\n\treturn false;}\n")
    else:
        c.write(" }\n")
    c.write("#undef State")

    c.write("\n\nstatic void nextState(unsigned char _state, unsigned char _interval) {\n")
    c.write("\trunOnce = true;\n")
    c.write("\texitFlag = false;\n")
    c.write("\tif(_interval) {\n")
    c.write("\t\tenabled = false;\n")
    c.write("\t\t" + new_file_name + "T = _interval; } \n")
    c.write("\tstate = _state; }\n")



with open(folder + new_file_name + ".h", "w") as h:
    if smType == "nested":
        h.write("#ifndef " + new_file_name + "T\n")
        h.write("#error " + new_file_name + "T is not defined, define this as the parent's timer\n")
        h.write("#endif\n\n") 


    h.write("enum " + new_file_name + "States {")
    h.write("\n\t" + new_file_name + "IDLE")
    for state in states:
        h.write(",\n\t"+ state)
    h.write(" };\n\n")
    
    h.write("static void nextState(unsigned char, unsigned char);\n")
    if smType == "nested":
        h.write("extern bool " + new_file_name + "(void); // nested state machines must resturn a '1' to signal that they are ready\n")            # function call to the state machine
    else:
        h.write("extern void " + new_file_name + "(void); \n")
    h.write("extern void " + new_file_name + "Stop(void);\n")     # stop
    h.write("extern void " + new_file_name + "SetState(unsigned char);\n")
    h.write("extern unsigned char " + new_file_name + "GetState(void);\n") # get currentState
    h.close()
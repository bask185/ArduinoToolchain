#!/usr/bin/python 
import sys
import xml


arrowsOut1 = []
arrowsIn1 = []
arrowsOut = []
arrowsIn = []
states1 = []
states = []

file_name = sys.argv[1]
#language = sys.argv[2] # 'c' or 'assembly'


smType = "Is this state machine a 'main' or 'nested' type state machine?"
print(smType)
while smType != "main" and smType != "nested":
    smType = input()
print(smType + " type selected")

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
    
#for arrowOut in arrowsOut1:
#    print(arrowOut)

#print("\r\n")

#for arrowIn in arrowsIn1:
    #print(arrowIn)

f.close()
#print(states)

new_file_name = file_name.split('.')
new_file_name = new_file_name[0]

if smType == "main":
    folder = "mainStateMachines/"
else:
    folder = "subStateMachines/"

with open(folder + new_file_name + ".cpp", "w") as c:
    
    c.write('#include "' + new_file_name + '.h"\n')
    c.write('#include "BIM/scheduler.h"\n')
    c.write('#include "BIM/io.h"\n\n')

    c.write("#define entryState if(runOnce) \n")
    c.write("#define onState runOnce = false; if(!runOnce)\n")
    c.write("#define exitState if(!exit) return false; else\n\n")
    
    c.write("static uint8 state = " + new_file_name + "IDLE;\n")
    c.write("static bool enabled, runOnce, exit;\n\n")
            
    c.write("extern void " + new_file_name + "Stop(void) {\n")     # stop
    c.write("\tstate = " + new_file_name + "IDLE; }\n\n")
            
    c.write("extern void " + new_file_name + "SetState(uint8 _state) {\n") # get currentState
    c.write("\tstate = _state;\n")
    c.write("\trunOnce = true;\n")
    c.write("\tenabled = true;}\n\n")

    c.write("extern uint8 " + new_file_name + "GetState(void) {\n") # get currentState
    c.write("\treturn state;}\n\n")

    
    c.write("#define State(x) static bool x##F(void)")
    for state in states:    # print all state functions
        c.write("\nState(" + state + ") {\n") 
        c.write("\tentryState{\n\t\t\n\t")
        c.write("}\n\tonState{\n\n\t\texit = true; }\n\t")
        c.write("return true; } }\n\n")
        
    c.write("#undef State\n\n")
    c.write("#define State(x) break; case x: if(x##F())")    
    c.write("\nextern bool " + new_file_name + "(void) {\n")
    c.write("\tif(enabled) switch(state){\n")
    c.write("\t\tdefault: case " + new_file_name + "IDLE: return true;\n\n")
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
    c.write("\telse if(!" + new_file_name + "T) enabled = true;\n")
    c.write("\treturn false;}\n")
    c.write("#undef State")

    c.write("\n\nstatic void nextState(uint8 _state, uint8 _interval) {\n")
    c.write("\tstate = _state;\n")
    c.write("\texit = false;\n")
    c.write("\t" + new_file_name + "T = _interval;\n")
    c.write("\tif(_interval) enabled = false;\n")
    c.write("\trunOnce = true; }")
    



with open(folder + new_file_name + ".h", "w") as h:
    h.write('#include "BIM/types.h"\n\n')

    if smType == "nested":
        h.write("#ifndef " + new_file_name + "T\n")
        h.write("#error " + new_file_name + "T is not defined, define this as the parent's timer\n")
        h.write("#endif\n\n") 


    h.write("enum " + new_file_name + "States {")
    h.write("\n\t" + new_file_name + "IDLE")
    for state in states:
        h.write(",\n\t"+ state)
    h.write(" };\n\n")
    
    h.write("static void nextState(uint8, uint8);\n")
    h.write("extern bool " + new_file_name + "(void); // nested state machines must resturn a '1' to signal that they are ready\n")            # function call to the state machine
    h.write("extern void " + new_file_name + "Stop(void);\n")     # stop
    h.write("extern void " + new_file_name + "SetState(uint8);\n")
    h.write("extern uint8 " + new_file_name + "GetState(void);\n") # get currentState
    h.close()
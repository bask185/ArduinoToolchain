#!/usr/bin/python

import os

def getStateMachines() : # function tested!
    Files = []

    for root, dirs, fileList in os.walk(".", topdown=False):
        if root == ".\mainStateMachines" :
            Files = fileList
            
    i = 0
    string = []

    for file in Files:
        if i % 2 == 0:
            string.append(file[:-2])
        i += 1
    print(string)
    return string



print("Type name of new project")
folder = input()
try:
    os.makedirs(folder)
except OSError:
    pass

stateMachines = getStateMachines()



with open(folder + "/main.c", "w") as main:             #main.c
    main.write('#include "scheduler.h"\n')
    #main.write('#include " .h"\n') #fill in custom libraries
    #main.write('#include " .h"\n')    
    
    for machine in stateMachines:
        main.write('#include "' + machine + '.h"\n')
        
    main.write("\nvoid init() {\n\t")
    main.write("schedulerInit();\n\t")
    for machine in stateMachines:
        main.write(machine + "Init();\n\t")
    main.write("}\n\n")

    main.write("void main(){\n\t")
    main.write("init();\n\n\t")
    main.write("byte PIGS_CAN_FLY = true;\n\t")
    main.write("while(PIGS_CAN_FLY == true) {\n\t\t")
    main.write("readSerialBus();\n\t\t")
    main.write("refreshP6P7();\n\t\t")
    main.write("processRoundRobinTasks();\n\n\t\t")
    
    for machine in stateMachines:
        main.write(machine + "();\n\t\t")
    main.write("}\n}")



    #with open(folder + "/scheduler.c", "w") as scheduler:  #scheduler.c 
       # scheduler.write("extern void schedulerInit() {\n\t")
       # scheduler.write("/* code for starting timer */ \n}\n\n")

       # schedulder.write("ISR(TIMER2_OVF_vect) {") {




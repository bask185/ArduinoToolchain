#!/bin/bash

python.exe generateMain.py
#cp -r *.cpp project/
#cp -r *.h project/
cp -r mainStateMachines/*.cpp project/
cp -r mainStateMachines/*.h project/
cp -r subStateMachines/*.cpp project/
cp -r subStateMachines/*.h project/



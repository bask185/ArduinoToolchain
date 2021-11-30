#!/usr/bin/env python

import subprocess
import shutil, errno
import os
import time
from datetime import datetime


versionNumber = input("Which version will you be releasing?\n")
time.sleep( 0.5 )


print("Enter a description for the changelog, send 'DONE' when ready")

commitMessage = ""

dateTimeObj = datetime.now()
with open("changelog.txt", "a") as changelog: 
    changelog.write( versionNumber + "   Date/time:" + str(dateTimeObj) + "\n" )
    
    newLine = ""
    while( newLine != 'DONE'):
        newLine = input()
        if( newLine != 'DONE' ):
            commitMessage += newLine + "\n"
            changelog.write(newLine + "\n")
    
    changelog.write("\n")
    changelog.close()

with open("src/version.h", "w") as file:
    file.write( "const char * version = \"" + versionNumber + "\" ;")

os.system("python src/build.py")            # Build project, before commiting and releasing

with open("src/temp.txt", "a") as temp: 
    temp.write(commitMessage)
    temp.close()

os.system("git add changelog.txt")
os.system('git commit -a -F "src/temp.txt"')
os.system('rm src/temp.txt')
os.system("git tag " + versionNumber )

#!/usr/bin/env python

import subprocess
import shutil, errno
import os
import time
from datetime import datetime


versionNumber = input("Which version will you be releasing?\n")
time.sleep( 0.5 )


print("Enter a description for the changelog, send 'DONE' when ready")

dateTimeObj = datetime.now()
with open("../changelog.txt", "a") as changelog: 
    changelog.write(str(dateTimeObj) + "   " + versionNumber + "\n" )
    
    newLine = ""
    while( newLine != 'DONE'):
        newLine = input()
        if( newLine != 'DONE' ):
            changelog.write(newLine + "\n")
    
    changelog.write("\n")
    changelog.close()

os.system("git tag " + versionNumber )





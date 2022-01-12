#!/usr/bin/env python

import os
from datetime import datetime


def getVersion():
    versionNumber = ""
    versionNumber = input("Which version will you be releasing?\n")
    print("VERSION " + versionNumber + " SELECTED\n" )
    return versionNumber



def getCommitText() :
    commitMessage = ""
    print("Enter a description for the changelog, send 'done' when ready")
    dateTimeObj = datetime.now()

        #with open("changelog.txt", "w") as changelog: 
            #changelog.write( versionNumber + "   Date/time:" + str(dateTimeObj) + "\n" )
    newLine = ""
    while( newLine != 'done'):
        newLine = input()
        if( newLine != 'done' ):
            commitMessage += newLine + "\n"

    answer = input("Do you wish to update changelog.txt? [y/n]\n")
    if answer == 'y' or answer == 'Y':
        os.system("git log --no-walk --tags --decorate=short | grep -v ^Author:  > changelog.txt")
        os.system("git add changelog.txt")                                          # just in case
        print("CHANGELOG.TXT UPDATED")
    else:
        print( "CHANGELOG.TXT NOT UPDATED")

    with open("src/temp.txt", "a") as temp: 
        temp.write(commitMessage)
        temp.close()



def updateVersionNumber():
    with open("src/version.h", "w") as file:
        file.write( "const char * version = \"" + versionNumber + "\" ;")



def buildProject():
    os.system("python src/build.py")            # Build project, before commiting and releasing



def commit():
    answer = input("Do you wish to perform a git commit?[y/n] (recommended for version.h)\n")
    if answer == 'y' or answer == 'Y':
        os.system('git commit -a -F "src/temp.txt"')
        print("COMMIT MADE\n")
    else:
        print("NO COMMIT MADE\n")
    os.system('rm src/temp.txt')



def tag():
    print("MAKING GIT TAG " + versionNumber)
    os.system("git tag " + versionNumber )



def push():
    answer = input("Do you wish to push to git server?[y/n] \n")
    if answer == 'y' or answer == 'Y':
        os.system('git push')
        print("REPOSITORY PUSHED\n")
    else:
        print("REPOSITORY NOT PUSHED\n")


################# MAIN PROGRAM #################
versionNumber = getVersion()
getCommitText()
updateVersionNumber()
buildProject()
commit()
tag()
push()

print("\nNEW VERSION SUCCUSFULLY RELEASED!")

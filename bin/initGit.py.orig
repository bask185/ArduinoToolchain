#!/usr/bin/env python

import os

#with open("README.md", "w") as f: OBSOLETE, replaced by gh.exe
#    message = input("Enter a line for the README file\n")
#    f.write( message )

os.system('git init')

with open(".git/info/exclude", "a") as f:
    f.write("\ndate.h\n")
    f.write("io.cpp\n")
    f.write("io.h\n")
    f.write("*.hex\n")
    f.write("*.bin\n")
    f.write("*.elf\n")
    f.write("*.eep\n")
<<<<<<< HEAD
    f.write("*.kicad_prl\n")
    f.write("*.kicad_pcb-bak\n")
    f.write("*.sch-bak\n")
    f.write("fp-info-cache\n")
=======
    f.write("*cache\n")
>>>>>>> 9b4d8bac8d92354d9d1ad6b35cee7416ee7bb4b5
    f.write("*.zip\n")

os.system('git add --all')
os.system('git commit -m "first commit"')
os.system('git branch -M master')
os.system('git tag v0.0.0')
print("\nrepository created!\n")

answer = ""
while answer != 'y' and answer != 'n':
    answer = input("Do you wish to push this repo to github.com?[y/n] (requires gh.exe to be installed\n")
    if answer == 'y':
        os.system('gh repo create')
    if answer == 'n':
        print( "\n\nTo push branch to github.com:" )
        print( "git remote add origin https://github.com/user_name/repository_name.git" )# argument 1 is username, argument 2 is repository name
        print( "git push -u origin master" )
        print( "\nOR\n" )
        print( "use 'gh repo create'")
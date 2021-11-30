#!/usr/bin/env python

import os

with open("README.md", "w") as f:
    message = input("Enter a line for the README file\n")
    f.write( message )

os.system('git init')
os.system('git add --all')
os.system('git commit -m "first commit"')
os.system('git branch -M master')
os.system('git tag v0.0.0')

print( "\n\nTo push branch to github.com:" )
print( "git remote add origin https://github.com/user_name/repository_name.git" )# argument 1 is username, argument 2 is repository name
print( "git push -u origin master" )
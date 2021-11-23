#!/bin/bash

echo "# demo" >> README.md
git init
git add --all
git commit -m "first commit"
git branch -M master

echo "To push branch to github.com:"
echo "git remote add origin https://github.com/user_name/repository_name.git" # argument 1 is username, argument 2 is repository name
echo "git push -u origin master"
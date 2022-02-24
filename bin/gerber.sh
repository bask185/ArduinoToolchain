#!/bin/bash



result=${PWD##*/}          # to assign to a variable

printf '%s\n' "${PWD##*/}" # to print to stdout
                           # ...more robust than echo for unusual names
                           #    (consider a directory named -e or -n)
mkdir "${PWD##*/}" 

mv *.drl "${PWD##*/}"
mv *.gbr "${PWD##*/}"

7z.exe a "${PWD##*/}".zip ./"${PWD##*/}"/*

rm -r "${PWD##*/}"
rm *.gbrjob
rm *.bak
rm *.lib





#!/usr/bin/env python
import os
import subprocess
import json 

# data = subprocess.check_output("arduino-cli board list --format json")

# dataJson = json.loads( data)

# for item in dataJson:
#   print(item," : ",dataJson[item]) 

COM = input("ENTER COM PORT")

retCode = os.system("arduino-cli burn-bootloader -b arduino:avr:nano -p "+ COM +" -P arduinoasisp")

if retCode == 1:
    print("BURNING BOOTLOADER SUCCES!!! ")
else:
    print("BOOTLOADER FAILED!!! ")

#!/usr/bin/env python

import os

print("\nDOWNLOADING AND INSTALL ARDUINO_CLI USING CURL\n")
os.system("curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh")
print("\nINITIALIZING ARDUINO CLI\n")
os.system("arduino-cli config init")
print("\nUPDATING CORES\n")
os.system("arduino-cli core update-index")
print("\nINSTALLING CORES FOR ORIGINAL ARDUINO BOARDS\n")
os.system("arduino-cli core listall arduino:avr")

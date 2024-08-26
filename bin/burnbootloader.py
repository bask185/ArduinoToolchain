#!/usr/bin/env python
import os
import subprocess
import json
import serial.tools.list_ports

def find_ch340_port():
    ports = list(serial.tools.list_ports.comports())
    for port in ports:
        if 'CH340' in port.description:
            return port.device
    return None

# Automatically find the CH340 port
COM = find_ch340_port()

if COM:
    print(f"Found CH340 USB TTL converter on {COM}")
    retCode = os.system(f"arduino-cli burn-bootloader -b arduino:avr:nano -p {COM} -P arduinoasisp")
    
    if retCode == 0:
        print("BURNING BOOTLOADER SUCCESS!!!")
    else:
        print("BOOTLOADER FAILED!!!")
else:
    print("CH340 USB TTL converter not found!")

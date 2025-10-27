#!/usr/bin/env python3
# Global upload script: compile then upload via ICSP using Arduino as ISP.

import os
import sys
import subprocess
import serial.tools.list_ports

FQBN       = os.getenv("FQBN", "arduino:avr:nano")   # target board
PROGRAMMER = os.getenv("PROGRAMMER", "arduinoasisp") # Arduino as ISP
SKETCH     = os.getenv("SKETCH", ".")                # current folder

def run(cmd):
    p = subprocess.run(cmd, text=True)
    return p.returncode

def get_com_port():
    # Pick first port that looks like an Arduino/CH340
    for port in serial.tools.list_ports.comports():
        desc = (port.description or "").lower()
        if "arduino" in desc or "ch340" in desc or "usb serial" in desc:
            return port.device
    return None

port = get_com_port()
if not port:
    print("No Arduino ISP detected (COM port not found).")
    sys.exit(1)

print(f"Using programmer on {port}")

print("COMPILING")
code = run(["arduino-cli", "compile", "-b", FQBN, SKETCH])
if code != 0:
    print("BUILD FAILED!!!")
    sys.exit(1)

print("UPLOADING (ICSP via programmer)")
# Important: --programmer selects ICSP; -p selects the programmer's serial port
code = run([
    "arduino-cli", "upload",
    "-b", FQBN,
    "-p", port,
    "--programmer", PROGRAMMER,
    SKETCH
])

if code != 0:
    print("UPLOADING FAILED!!!")
    sys.exit(1)

print("UPLOADING SUCCESS!!!")
sys.exit(0)

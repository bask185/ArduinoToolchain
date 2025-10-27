#!/usr/bin/env python3
# Global build script (no -e / no export)
# Compiles the current sketch folder for Arduino Nano.

import os
import sys
import subprocess

FQBN = os.getenv("FQBN", "arduino:avr:nano")   # override via env if needed
SKETCH = os.getenv("SKETCH", ".")              # current folder

def run(cmd):
    p = subprocess.run(cmd, text=True)
    return p.returncode

print("BUILDING PROJECT")
code = run(["arduino-cli", "compile", "-b", FQBN, SKETCH])
if code != 0:
    print("BUILD FAILED!!!")
    sys.exit(1)

print("BUILD SUCCESS!!!")
sys.exit(0)

#!/usr/bin/env python3
# Global upload script with optional conf.ini
# Defaults if no conf.ini:
#   uploader   = icsp
#   fqbn       = arduino:avr:nano
#   programmer = arduinoasisp
#
# USB mode : arduino-cli upload -b <FQBN> -p <COM> .
# ICSP mode: arduino-cli upload -b <FQBN> -p <COM> --programmer <PROGRAMMER> .

import os
import sys
import subprocess
from pathlib import Path
import serial.tools.list_ports

DEFAULT_UPLOADER = "icsp"          # usb | icsp
DEFAULT_FQBN     = "arduino:avr:nano"
DEFAULT_PROGRAMMER = "arduinoasisp"

BOARD_MAP = {
    "nano": "arduino:avr:nano",
    "attiny85": "ATTinyCore:avr:attinyx5",  # Requires ATTinyCore or similar
}

def read_conf_ini(conf_path: Path):
    cfg = {}
    if not conf_path.exists():
        return cfg
    text = conf_path.read_text(encoding="utf-8").splitlines()
    for line in text:
        line = line.strip()
        if not line or line.startswith("#") or line.startswith(";") or line.startswith("["):
            continue
        if "=" in line:
            k, v = line.split("=", 1)
            cfg[k.strip().lower()] = v.strip()
        elif ":" in line:
            k, v = line.split(":", 1)
            cfg[k.strip().lower()] = v.strip()
    return cfg

def resolve_fqbn(cfg: dict) -> str:
    if "fqbn" in cfg and cfg["fqbn"]:
        return cfg["fqbn"]
    if "board" in cfg and cfg["board"]:
        key = cfg["board"].strip().lower()
        if ":" in key:
            return cfg["board"]
        if key in BOARD_MAP:
            return BOARD_MAP[key]
        print(f"Warning: Unknown board shorthand '{cfg['board']}'. Falling back to default {DEFAULT_FQBN}.")
    return DEFAULT_FQBN

def resolve_uploader(cfg: dict) -> str:
    val = cfg.get("uploader", DEFAULT_UPLOADER).strip().lower()
    return "usb" if val == "usb" else "icsp"

def resolve_programmer(cfg: dict) -> str:
    return cfg.get("programmer", DEFAULT_PROGRAMMER).strip().lower() or DEFAULT_PROGRAMMER

def get_com_port():
    """
    Detect the first Arduino-like COM port.
    """
    for port in serial.tools.list_ports.comports():
        desc = (port.description or "").lower()
        if "arduino" in desc or "ch340" in desc or "usb serial" in desc:
            return port.device
    return None

def run(cmd):
    return subprocess.run(cmd, text=True).returncode

def main():
    project_dir = Path.cwd()
    conf = read_conf_ini(project_dir / "conf.ini")

    fqbn       = resolve_fqbn(conf)
    uploader   = resolve_uploader(conf)         # usb | icsp
    programmer = resolve_programmer(conf)       # only used when uploader=icsp

    port = get_com_port()
    if not port:
        print("No Arduino device/programmer detected on any COM port.")
        sys.exit(1)

    print(f"COMPILING (FQBN={fqbn})")
    code = run(["arduino-cli", "compile", "-b", fqbn, str(project_dir)])
    if code != 0:
        print("BUILD FAILED!!!")
        sys.exit(1)

    if uploader == "usb":
        print(f"UPLOADING via USB on {port}")
        cmd = ["arduino-cli", "upload", "-b", fqbn, "-p", port, str(project_dir)]
    else:
        print(f"UPLOADING via ICSP (programmer={programmer}) on {port}")
        cmd = ["arduino-cli", "upload", "-b", fqbn, "-p", port, "--programmer", programmer, str(project_dir)]

    code = run(cmd)
    if code != 0:
        print("UPLOADING FAILED!!!")
        sys.exit(1)

    print("UPLOADING SUCCESS!!!")
    sys.exit(0)

if __name__ == "__main__":
    main()

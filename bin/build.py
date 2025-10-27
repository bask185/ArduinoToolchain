#!/usr/bin/env python3
# Global build script with optional conf.ini
# - Default FQBN: arduino:avr:nano
# - If conf.ini exists, it can override via 'fqbn' or 'board'

import os
import sys
import subprocess
from pathlib import Path

DEFAULT_FQBN = "arduino:avr:nano"

# Common shorthand -> FQBN mapping (extend as needed)
BOARD_MAP = {
    "nano": "arduino:avr:nano",
    # ATTinyCore example (requires the core to be installed):
    "attiny85": "ATTinyCore:avr:attinyx5",
}

def read_conf_ini(conf_path: Path):
    """
    Read simple key=value lines from conf.ini (sectionless or with sections).
    Returns a dict with lowercase keys.
    """
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
    # Priority: fqbn key > board key > default
    if "fqbn" in cfg and cfg["fqbn"]:
        return cfg["fqbn"]
    if "board" in cfg and cfg["board"]:
        key = cfg["board"].strip().lower()
        # If it looks like a full FQBN (contains ':'), just use it
        if ":" in key:
            return cfg["board"]
        # Else map shorthand
        if key in BOARD_MAP:
            return BOARD_MAP[key]
        print(f"Warning: Unknown board shorthand '{cfg['board']}'. Falling back to default {DEFAULT_FQBN}.")
    return DEFAULT_FQBN

def run(cmd):
    return subprocess.run(cmd, text=True).returncode

def main():
    project_dir = Path.cwd()
    conf = read_conf_ini(project_dir / "conf.ini")
    fqbn = resolve_fqbn(conf)

    print(f"BUILDING PROJECT (FQBN={fqbn})")
    code = run(["arduino-cli", "compile", "-b", fqbn, str(project_dir)])
    if code != 0:
        print("BUILD FAILED!!!")
        sys.exit(1)

    print("BUILD SUCCESS!!!")
    sys.exit(0)

if __name__ == "__main__":
    main()

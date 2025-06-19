#!/usr/bin/env python3
import os
import sys
import shutil
import time
from pathlib import Path

def fail(msg):
    print(f"[FAIL] {msg}")
    sys.exit(1)

def info(msg):
    print(f"[INFO] {msg}")

def success(msg):
    print(f"[OK] {msg}")

def main():
    software_dir = Path.cwd()
    repo_root = software_dir.parent
    repo_name = repo_root.name

    sketch_path = software_dir / "software.ino"
    build_dir = software_dir / "build"
    hex_file = build_dir / "arduino.avr.nano" / "software.ino.hex"
    release_dir = repo_root / "releases"

    if not sketch_path.exists():
        fail(f"Bestand niet gevonden: {sketch_path}")

    info("Compileren met arduino-cli...")
    result = os.system("arduino-cli compile -b arduino:avr:nano -e --fqbn arduino:avr:nano .")
    if result != 0:
        fail("Build gefaald!")

    if not hex_file.exists():
        fail(f".hex bestand niet gevonden: {hex_file}")

    release_dir.mkdir(exist_ok=True)

    target = release_dir / f"{repo_name}.hex"
    shutil.move(str(hex_file), str(target))
    success(f"HEX verplaatst naar: {target}")

    # Wacht even zodat Windows file locks vrijgeeft
    time.sleep(1)

    try:
        shutil.rmtree(build_dir)
        info("Build-folder volledig verwijderd.")
    except Exception as e:
        fail(f"Kon build folder niet verwijderen: {e}")

    success("Klaar!")

if __name__ == "__main__":
    main()

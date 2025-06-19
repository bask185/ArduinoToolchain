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

def try_remove_dir(path, retries=3, delay=1):
    for attempt in range(retries):
        try:
            shutil.rmtree(path)
            return True
        except Exception as e:
            if attempt < retries - 1:
                time.sleep(delay)
            else:
                fail(f"Kon map {path} niet verwijderen: {e}")
    return False

def try_remove_file_or_dir(path):
    try:
        if path.is_file() or path.is_symlink():
            path.unlink()
        elif path.is_dir():
            shutil.rmtree(path)
        success(f"{path.name} verwijderd.")
    except Exception as e:
        fail(f"Kon {path.name} niet verwijderen: {e}")

def main():
    hardware_dir = Path.cwd()
    root_dir = hardware_dir.parent
    releases_dir = root_dir / "releases"
    jlcpcb_dir = hardware_dir / "jlcpcb"
    production_dir = jlcpcb_dir / "production_files"
    fp_cache = hardware_dir / "fp-info-cache"  # FIXED: local ipv root

    # 1. Verwijder fp-info-cache
    if fp_cache.exists():
        try_remove_file_or_dir(fp_cache)
    else:
        info("fp-info-cache niet aanwezig – prima.")

    # 2. Bestanden zoeken
    if not production_dir.exists():
        fail("production_files map bestaat niet.")

    files_to_move = list(production_dir.glob("BOM-*.csv")) + \
                    list(production_dir.glob("CPL-*.csv")) + \
                    list(production_dir.glob("GERBER-*.zip"))

    if not files_to_move:
        fail("Geen BOM/CPL/GERBER bestanden gevonden in production_files.")

    info(f"Gevonden bestanden: {[f.name for f in files_to_move]}")

    releases_dir.mkdir(exist_ok=True)

    for f in files_to_move:
        dest = releases_dir / f.name
        info(f"Kopiëren: {f.name} -> {dest}")
        shutil.copy2(f, dest)
        success(f"Gekopieerd naar releases/: {f.name}")

    # 3. Verwijder jlcpcb folder
    if jlcpcb_dir.exists():
        if try_remove_dir(jlcpcb_dir):
            success("jlcpcb map en inhoud volledig verwijderd.")
    else:
        info("jlcpcb folder niet aanwezig – niets te doen.")

    success("Board file release afgerond.")

if __name__ == "__main__":
    main()

#!/usr/bin/env python3
import os
import sys
import shutil
import time
from pathlib import Path

def fail(msg):
    print("[FAIL] " + msg)
    sys.exit(1)

def info(msg):
    print("[INFO] " + msg)

def success(msg):
    print("[OK] " + msg)

def try_remove_dir(path, retries=3, delay=1):
    for attempt in range(retries):
        try:
            shutil.rmtree(path)
            return True
        except Exception as e:
            if attempt < retries - 1:
                time.sleep(delay)
            else:
                fail("Kon map niet verwijderen: " + str(e))
    return False

def try_remove_file_or_dir(path):
    try:
        if path.is_file() or path.is_symlink():
            path.unlink()
        elif path.is_dir():
            shutil.rmtree(path)
        success("Verwijderd: " + str(path))
    except Exception as e:
        fail("Kon " + str(path) + " niet verwijderen: " + str(e))

def main():
    hardware_dir = Path.cwd().resolve()
    project_dir = hardware_dir.parent
    releases_dir = project_dir / "releases"
    releases_dir.mkdir(exist_ok=True)

    for production_dir in hardware_dir.glob("**/jlcpcb/production_files"):
        if not production_dir.is_dir():
            continue

        jlcpcb_dir = production_dir.parent
        subproject_dir = jlcpcb_dir.parent

        # project_name = folder waarin jlcpcb zit (subproject of hardware zelf)
        try:
            relative = jlcpcb_dir.relative_to(hardware_dir)
            project_name = relative.parts[0] if len(relative.parts) > 0 else "hardware"
        except ValueError:
            project_name = "unknown"

        info("Verwerken: " + project_name)

        # 1. Verwijder lokale fp-info-cache als die bestaat
        fp_cache = subproject_dir / "fp-info-cache"
        if fp_cache.exists():
            try_remove_file_or_dir(fp_cache)
        else:
            info("Geen fp-info-cache aanwezig in: " + str(subproject_dir))

        # 2. Bestanden zoeken
        files_to_move = list(production_dir.glob("BOM-*.csv")) + \
                        list(production_dir.glob("CPL-*.csv")) + \
                        list(production_dir.glob("GERBER-*.zip"))

        if not files_to_move:
            info("Geen productie bestanden gevonden in: " + str(production_dir))
            continue

        for f in files_to_move:
            dest = releases_dir / f.name
            info("Gevonden bestand: " + str(f))
            info("Bestand wordt gekopieerd naar: " + str(dest))
            try:
                shutil.copy2(f, dest)
                success(f.name + " gekopieerd.")
            except Exception as e:
                fail("Kon bestand niet kopiëren: " + str(e))

        # 3. jlcpcb dir verwijderen
        if jlcpcb_dir.exists() and jlcpcb_dir.name == "jlcpcb":
            if try_remove_dir(jlcpcb_dir):
                success("Verwijderd jlcpcb dir in: " + str(subproject_dir))
        else:
            info("jlcpcb map niet gevonden of herkend")

    success("Klaar: alle productie bestanden verwerkt.")

if __name__ == "__main__":
    main()

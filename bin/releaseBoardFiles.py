#!/usr/bin/env python
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

def find_repo_root(path: Path) -> Path:
    """Walk up from path until we find the folder named 'hardware', then return its parent."""
    current = path.resolve()
    while current != current.parent:
        if current.name == "hardware":
            return current.parent
        current = current.parent
    return path.resolve()   # fallback: couldn't find hardware/ in ancestry

def main():
    sys.stdout.reconfigure(encoding='utf-8')
    start_dir = Path.cwd().resolve()

    for production_dir in start_dir.glob("**/jlcpcb/production_files"):
        if not production_dir.is_dir():
            continue

        jlcpcb_dir     = production_dir.parent
        subproject_dir = jlcpcb_dir.parent

        # Derive releases/ per repo, not once from cwd
        repo_root    = find_repo_root(jlcpcb_dir)
        releases_dir = repo_root / "releases"
        releases_dir.mkdir(exist_ok=True)

        # project_name = folder waarin jlcpcb zit (subproject of hardware zelf)
        hardware_dir = repo_root / "hardware"
        try:
            relative = jlcpcb_dir.relative_to(hardware_dir)
            project_name = relative.parts[0] if len(relative.parts) > 0 else hardware_dir.parent.name
        except ValueError:
            project_name = repo_root.name

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

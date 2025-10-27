#!/usr/bin/env python3
# updateManuals.py — Convert all .odt files in current directory to PDF via LibreOffice (soffice), ASCII-only output.

import subprocess
import sys
from pathlib import Path

def convert_odt_to_pdf(odt_file: Path):
    outdir = str(odt_file.parent)
    print(f"Converting {odt_file.name} -> PDF")

    cmd = [
        "soffice",
        "--headless",
        "--convert-to", "pdf",
        "--outdir", outdir,
        str(odt_file)
    ]

    try:
        subprocess.run(cmd, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except FileNotFoundError:
        print("Error: 'soffice' not found in PATH. Install LibreOffice or add it to PATH.")
        return False
    except subprocess.CalledProcessError as e:
        print(f"Conversion failed for {odt_file.name} (LibreOffice error {e.returncode})")
        return False

    out_file = odt_file.with_suffix(".pdf")
    if out_file.exists():
        print(f"OK: {out_file.name} created")
        return True
    else:
        print(f"Warning: conversion reported success but {out_file.name} not found")
        return False


def main():
    cwd = Path(".").resolve()
    odt_files = sorted(cwd.glob("*.odt"))

    if not odt_files:
        print("No .odt files found in current directory.")
        sys.exit(0)

    success = 0
    for f in odt_files:
        if convert_odt_to_pdf(f):
            success += 1

    print(f"\nDone. {success} of {len(odt_files)} file(s) converted successfully.")
    sys.exit(0 if success == len(odt_files) else 1)


if __name__ == "__main__":
    main()

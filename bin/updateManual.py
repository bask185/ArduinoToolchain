#!/usr/bin/env python3
# updateManual.py — Convert Manual.odt -> Manual.pdf via LibreOffice (soffice), ASCII-only output.

import subprocess
import sys
from pathlib import Path

def main():
    infile = Path("Manual.odt").resolve()
    outdir = str(infile.parent)

    if not infile.exists():
        print("Error: Manual.odt not found in {}".format(infile.parent))
        sys.exit(1)

    print("Converting {} -> PDF".format(infile.name))

    cmd = [
        "soffice",
        "--headless",
        "--convert-to", "pdf",
        "--outdir", outdir,
        str(infile)
    ]

    try:
        # Run soffice; if it fails, raise
        subprocess.run(cmd, check=True)
    except FileNotFoundError:
        print("Error: 'soffice' not found in PATH. Install LibreOffice or add it to PATH.")
        sys.exit(1)
    except subprocess.CalledProcessError as e:
        print("Conversion failed (LibreOffice error). Return code: {}".format(e.returncode))
        sys.exit(1)

    out_file = infile.with_suffix(".pdf")
    if out_file.exists():
        print("OK: {} created".format(out_file.name))
        sys.exit(0)
    else:
        print("Warning: conversion reported success but {} not found".format(out_file.name))
        sys.exit(1)

if __name__ == "__main__":
    main()

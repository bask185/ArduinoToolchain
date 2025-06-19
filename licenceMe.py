#!/usr/bin/env python3

import os

# Licentie informatie
license_text = """/*
 * Copyright (C) 2024 Sebastiaan Knippels, Train-Science
 *
 * To the extent possible under law, the person who associated CC0 with this work
 * has waived all copyright and related or neighboring rights to this work.
 *
 * This work is published from: The Netherlands
 *
 * You can copy, modify, distribute and perform the work, even for commercial purposes,
 * all without asking permission. See the full license for details at:
 * https://creativecommons.org/publicdomain/zero/1.0/
 */
"""

# Bestandsextensies die we willen aanpassen
extensions = ['.cpp', '.h']

# Lijst van bestanden die moeten worden uitgesloten
excluded_files = [
    "NmraDcc.cpp", "NmraDcc.h", "ln_buf.cpp", "ln_buf.h", "ln_config.h",
    "ln_sw_uart.cpp", "ln_sw_uart.h", "ln_opc.h", "utils.h", "utils.c",
    "LocoNet.h", "LocoNet.cpp", "XpressNetMaster.cpp", "XpressNetMaster.h"
]

def prepend_license_to_file(file_path):
    with open(file_path, 'r+') as file:
        content = file.read()
        file.seek(0, 0)
        file.write(license_text + '\n' + content)

def process_files_in_directory(directory):
    for root, _, files in os.walk(directory):
        for file in files:
            if any(file.endswith(ext) for ext in extensions) and file not in excluded_files:
                file_path = os.path.join(root, file)
                prepend_license_to_file(file_path)
                print(f"Licentie toegevoegd aan: {file_path}")
            else:
                print(f"Bestand overgeslagen: {file}")

if __name__ == "__main__":
    # Start in de huidige directory
    current_directory = os.getcwd()
    process_files_in_directory(current_directory)

#!/usr/bin/env python3
# Global serial monitor opener
# Opens Arduino serial monitor with automatic COM port detection.
# Default baudrate = 115200 bps

import sys
import subprocess
import serial.tools.list_ports

DEFAULT_BAUD = 115200

def get_com_port():
    """
    Automatically detect the first Arduino or CH340 device.
    Returns: COM port string (e.g. 'COM4') or None if not found.
    """
    for port in serial.tools.list_ports.comports():
        desc = (port.description or "").lower()
        if "arduino" in desc or "ch340" in desc or "usb serial" in desc:
            return port.device
    return None

def main():
    com_port = get_com_port()
    if not com_port:
        print("No Arduino device detected on any COM port.")
        sys.exit(1)

    print(f"Opening serial monitor on {com_port} @ {DEFAULT_BAUD} bps")

    # Start Arduino CLI monitor
    cmd = [
        "arduino-cli",
        "monitor",
        "-p", com_port,
        "-c", f"baudrate={DEFAULT_BAUD}"
    ]

    try:
        subprocess.run(cmd)
    except KeyboardInterrupt:
        print("\nSerial monitor closed by user.")
    except FileNotFoundError:
        print("Error: arduino-cli not found in PATH.")
        sys.exit(1)

if __name__ == "__main__":
    main()

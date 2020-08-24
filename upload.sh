#!/bin/bash
echo "COMPILING"
arduino-cli compile -p "COM4" -b arduino:avr:nano ~/Documents/software/piTrain-decoder

echo "UPLOADING"
arduino-cli upload -b arduino:avr:nano:cpu=atmega328old -p COM3 -i ~/Documents/software/piTrain-decoder/piTrain-decoder.arduino.avr.nano.hex
rm *.hex *.elf
exit

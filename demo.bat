@ECHO OFF

ECHO Downloading latest version of arduino cli
certutil.exe -urlcache -split -f "https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip" arduino-cli.zip

ECHO unzipping arduinio-cli.zip
unzip.exe arduino-cli.zip

ECHO updating arduino cli cores
arduino-cli core update-index

ECHO listing connected arduino boards
arduino-cli board list

ECHO installing avr cores for atmega328P support
arduino-cli core install arduino:avr

:: delete following step
:: ECHO compiling program 
:: arduino-cli compile --fqbn arduino:avr:uno test

ECHO uploading hex file to arduino
arduino-cli upload -p COM9 --fqbn arduino:avr:uno -i test.hex

ECHO finished
PAUSE


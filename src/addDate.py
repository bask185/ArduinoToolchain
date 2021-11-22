#!/usr/bin/python

from datetime import datetime

dateTimeObj = datetime.now()

with open("src/date.cpp", "w") as f:
    f.write("#include \"date.h\"\n\n")
    f.write("void printDate()\n")
    f.write("{\n")
    f.write("\tSerial.println(\"" + str(dateTimeObj) + "\") ;\n")
    f.write("}\n")


with open("src/date.h", "w") as f:
    f.write("#include <Arduino.h>\n\n")
    f.write("extern void printDate() ;")
    f.close()

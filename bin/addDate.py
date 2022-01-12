#!/usr/bin/python

from datetime import datetime

dateTimeObj = datetime.now()

with open("src/date.h", "w") as f:
    f.write( "const char * date = \"" + str(dateTimeObj)  + "\" ;")
    f.close()

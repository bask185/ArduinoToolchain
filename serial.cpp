#include "serial.h"

byte serialByte;


extern void readSerialBus() {
	if(Serial.available()) {
		serialByte = Serial.read();
	}
	else {
		serialByte = 0;
	}
}

/*	switch(serialByte) { // your own serial protocol can be handled here.
	case //const :
	break;

	case //const1 :
	break;

	case //const2 :
	break;
}
*/

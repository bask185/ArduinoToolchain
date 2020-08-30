

// enum servoStates {
//     movingLeft,
//     movingRight,
// } ;

#include "ServoSweep.h"

#define movingLeft 0
#define movingRight 1

ServoSweep::ServoSweep( byte _min, byte _max, byte _speed ) {                   // constructor 1

    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;
   
    middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;               // start with middle position

    pos = middlePosition ;
}

ServoSweep::ServoSweep( byte _min, byte _max, byte _speed, byte _relayPin ) {      // constructor 2
    
    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;

    middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;

    pos = middlePosition ;

    relayPresent = 1;
    relayPin = _relayPin ;
    pinMode( relayPin, OUTPUT ) ;

}


uint8_t ServoSweep::sweep () {
    //currentTime = millis() ;

    if( millis() > timeToRun ) {
        timeToRun = millis() + servoSpeed ;

        if( state ) {
            if( pos < servoMax ) pos ++ ;
        }
        else {
            if( pos > servoMin ) pos -- ;
        }

        if( prevPos != pos ) {
            prevPos  = pos ;

            if( relayPresent == 1 ) {
                if( pos < middlePosition ) digitalWrite( relayPin,  LOW ) ;
                else                       digitalWrite( relayPin, HIGH ) ;
            }
            return pos ;
        }
        else {
            return 0 ;
        }
    }
}

void ServoSweep::setState( uint8_t _state ) {
    state = _state ;
}



// enum servoStates {
//     movingLeft,
//     movingRight,
// } ;

#include "ServoSweep.h"

#define movingLeft 0
#define movingRight 1

ServoSweep::ServoSweep( byte _servoPin, byte _min, byte _max, byte _speed ) {                   // constructor 1
    
    servoPin = _servoPin ;
    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;
   
    middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;               // start with middle position

    pos = middlePosition ;

    servo.write( pos ) ;
    servo.attach( servoPin ) ;
}

ServoSweep::ServoSweep( byte _servoPin, byte _min, byte _max, byte _speed, byte _relayPin ) {      // constructor 2
    
    servoPin = _servoPin ;
    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;

    middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;

    pos = middlePosition ;

    servo.write( pos ) ;
    servo.attach( servoPin ) ;

    relayPresent = 1;
    relayPin = _relayPin ;
    pinMode( relayPin, OUTPUT ) ;

}


void ServoSweep::sweep () {
    unsigned long currentTime = millis() ;

    if( currentTime > timeToRun ) {
        timeToRun = currentTime + servoSpeed ;

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
            servo.write( pos ) ;
        }
    }
}

void ServoSweep::setState( uint8_t _state ) {
    state = _state ;
}

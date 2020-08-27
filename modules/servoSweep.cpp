

// enum servoStates {
//     movingLeft,
//     movingRight,
// } ;

#include "ServoSweep.h"

#define movingLeft 0
#define movingRight 1

void ServoSweep( byte _servoPin, byte _inputPin, byte _min, byte _max, byte _speed ) {                   // constructor 1
    
    servoPin = _servoPin ;
    inputPin = _inputPin ;
    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;

    if( digitalRead( inputPin ) == true ) state = movingLeft ;
    else                                  state = movingRight ;
    
    middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;               // start with middle position

    pos = middlePosition

    pinMode( inputPin, INPUT_PULLUP ) ;
    servo.write( pos ) ;
    servo.attach( servoPin ) ;
}

void ServoSweep( byte _servoPin, byte _inputPin, byte _min, byte _max, byte _speed, byte _relayPin ) {      // constructor 2
    
    servoPin = _servoPin ;
    inputPin = _inputPin ;
    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;

    if( digitalRead( inputPin ) == true ) state = movingLeft ;
    else                                  state = movingRight ;

    middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;

    pos = middlePosition

    pinMode( inputPin, INPUT_PULLUP ) ;
    servo.write( pos ) ;
    servo.attach( servoPin ) ;

    relayPresent = 1;
    relayPin = _relayPin ;
}


void ServoSweep::sweep ( ) {
    unsigned long currentTime = millis() ;

    if( currentTime > timeToRun ) {
        timeToRun = currentTime + servoSpeed ;

        switch( state ) {

        case movingRight:
            if( pos < servoMax ) pos ++ ;
            if( digitalRead( inputPin ) ) state = movingLeft ;
            break ;

        case movingLeft:
            if( pos > servoMin ) pos -- ;
            if( !digitalRead( inputPin ) ) state = movingRight ;
            break ;
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
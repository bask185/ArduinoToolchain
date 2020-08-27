#include "servoSweep.h"


void ServoSweep( byte _pin, byte _input, byte _min, byte _max, byte _speed ) {
    
    servoPin = _pin ;
    inputPin = _input ;
    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;

    state = movingLeft ;

    pos = ( servoMax - servoMin ) / 2 + servoMin ; // start with middle position

    pinMode( inputPin, INPUT_PULLUP ) ;
    servo.write( pos ) ;
    servo.attach( servoPin ) ;
}


void ServoSweep::sweep ( byte state ) {
    unsigned long currentTime = millis() ;

    if( currentTime > timeToRun ) {
        timeToRun = currentTime + servoSpeed ;

        switch( state ) {

        case moveRight:
            if( pos < servoMax ) pos ++ ;
            if( digitalRead( inputPin ) ) state = moveLeft ;
            break ;

        case moveLeft:
            if( pos > servoMin ) pos -- ;
            if( !digitalRead( inputPin ) ) state = moveRight ;
            break ;
        }

        if( prevPos != pos ) {
            prevPos  = pos ;
           
            servo.write( pos ) ;
        }
    }
}
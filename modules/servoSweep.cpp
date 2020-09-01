#include "ServoSweep.h"

#define movingLeft 0
#define movingRight 1

ServoSweep::ServoSweep( uint8_t _servoPin, uint8_t _min, uint8_t _max, uint8_t _turnOff,  uint8_t _speed ) {                   // constructor 1
    
    servoPin = _servoPin ;
    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;
   
    middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;               // start with middle position
    pos = middlePosition ;

    if( _turnOff ) turnOff = 1 ;
    else           turnOff = 0 ;
}

ServoSweep::ServoSweep( uint8_t _servoPin, uint8_t _min, uint8_t _max, uint8_t _speed, uint8_t _turnOff, uint8_t _relayPin ) {      // constructor 2
    
    servoPin = _servoPin ;
    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;

    middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;

    pos = middlePosition ;

    relayPresent = 1;
    relayPin = _relayPin ;

    if( _turnOff ) turnOff = 1 ;
    else           turnOff = 0 ;
}

void ServoSweep::begin() {
    if( servoPin != 255) {
        servo.write( pos ) ;
        servo.attach( servoPin ) ;
    }

    if( relayPresent ) pinMode( relayPin, OUTPUT ) ;
}

void ServoSweep::setState( uint8_t _state ) {
    state = _state ;
    if( turnOff ) servo.attach( servoPin ) ;
}

// void ServoSweep::turnOn() {
//     servo.attach( servoPin ) ;
// }

// void ServoSweep::turnOff() {
//     servo.detach(  ) ;
// }

uint8_t ServoSweep::sweep ( ) {
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

            if( ( pos == servoMax || pos == servoMin ) && turnOff == 1 ) servo.detach( ) ;

            if( relayPresent ) {
                if( pos < middlePosition ) digitalWrite( relayPin,  LOW ) ;
                else                       digitalWrite( relayPin, HIGH ) ;
            }
            if( servoPin != 255 ) servo.write( pos ) ;
            return pos ;
        }
        else {
            return 0 ;
        }
    }
}
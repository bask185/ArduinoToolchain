#include "turnout.h"

Turnout::Turnout( uint8_t _relayPin ) {
    relayPin = _relayPin ;
    servoPos = 0 ;
}

void Turnout::setState( uint8_t _state ) {
    state = _state ;
}

void Turnout::update() {

    if( state == straight && servoPos < servoMax ) servoPos ++ ; // inc pos
    if( state == curved   && servoPos > servoMin ) servoPos -- ; // dec pos
    
    if( servoPos == middlePos ) digitalWrite( relayPin, !digitalRead( relayPin ) ) ; // toggle relay when servo is halfway

    if( servoPos == servoMin 
    || servoPos == servoMax ) return 0 ;                    // end positions means no new servo updates, use the value 0 not to write a position to a servo
    else                      return servoPos ;
}
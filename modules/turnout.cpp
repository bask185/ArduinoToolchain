/*
 * Copyright (C) 2024 Sebastiaan Knippels, Train-Science
 *
 * To the extent possible under law, the person who associated CC0 with this work
 * has waived all copyright and related or neighboring rights to this work.
 *
 * This work is published from: The Netherlands
 *
 * You can copy, modify, distribute and perform the work, even for commercial purposes,
 * all without asking permission. See the full license for details at:
 * https://creativecommons.org/publicdomain/zero/1.0/
 */

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
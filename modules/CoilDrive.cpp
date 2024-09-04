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

#include "CoilDrive.h"

CoilDrive::CoilDrive( uint8 _pinA, uint8 _pinB, uint32 _switchTime )
{
    pinA = _pinA ;
    pinB = _pinB ;
    switchTime = _switchTime ;
}

void CoilDrive::begin()
{
    pinMode( pinA, OUTPUT ) ;
    pinMode( pinB, OUTPUT ) ;
}

void CoilDrive::setState( uint8 _state )
{
    prevTime = millis() ;
    set = true ;

    if( _state ) digitalWrite( pinA, HIGH ) ;
    else         digitalWrite( pinB, HIGH ) ;
}

void CoilDrive::update()
{
    if( set == true && (millis() - prevTime) >= switchTime )
    {   set  = false ;
        
        digitalWrite( pinA, LOW ) ;
        digitalWrite( pinB, LOW ) ;
    }
}
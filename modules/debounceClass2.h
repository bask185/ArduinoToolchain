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

#ifndef DEBOUNCECLASS2_H
#define DEBOUNCECLASS2_H

#include <Arduino.h>
#include "macros.h"

// note LOW, HIGH, FALLING and RISING are already defined in Arduino.h
const int  BOTH = 1 ;
const int SHORT = 1 ;
const int  LONG = 2 ;

class Debouncer
{
public:
    Debouncer() ;

    void    setFlank( uint8 ) ;            // use this with RISING, FALLING or BOTH. This may set Q and the return value of debounce() appropiately
    void    setPin( uint8 ) ;              // enables use of digitalRead()
    void    setDebounceTime( uint32 ) ;    // enables internal use of millis() to set debounce intervals
    uint8   pressTime( uint32, uint8 ) ;
    uint8   hasRisen() ;
    uint8   hasFallen() ;
    uint8   debounce() ;
    uint8   debounce( uint8 ) ;

    uint16          state : 2 ;
    uint16      oldSample : 1 ;
    uint16            pin : 6 ;
    uint16      statePrev : 1 ;
    uint16       newState : 2 ;
    uint16          flank : 2 ;
    uint16              Q : 1 ;
    uint16 hasLongPressed : 1 ;
    uint16   hasRisenFlag : 1 ;
    uint16  hasFallenFlag : 1 ;

private:
    uint32 prevTime ;
    uint32 interval ;
    uint32 pressStartTime ;
} ;

extern void stateChange( Debouncer* btn, uint8 state )  __attribute__ ((weak)) ;
extern void  shortPress( Debouncer* btn )               __attribute__ ((weak)) ;
extern void   longPress( Debouncer* btn )               __attribute__ ((weak)) ;

#endif

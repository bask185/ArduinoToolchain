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

class Debouncer
{
public:
    static constexpr uint32 DEFAULT_INTERVAL = 20;  // Voeg een compile-time constante toe voor het default interval

    Debouncer() ;                                   // default debounce interval van 20ms
    Debouncer( uint8 ) ;                            // default debounce interval van 20ms
    Debouncer( uint8, uint32 ) ;    
    Debouncer( uint32 ) ;

    void debounce() ;
    void debounce( uint8 ) ;

    uint8     state : 2 ;
    uint8 oldSample : 1 ;
    uint8       pin : 6 ;
    uint8 statePrev : 1 ;
    uint8  newState : 2 ;

private:
    uint32 prevTime ;
} ;

#endif

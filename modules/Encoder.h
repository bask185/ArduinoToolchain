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

#include <Arduino.h>
#include "debounceClass.h"

#ifndef ENCODER_H
#define ENCODER_H

#define leftPulse 19    // just random nummers which arent HIGH, LOW, RISING OR FALLING
#define rightPulse 20

class Encoder {
public: 
/****** CONSTRUCTOR ******/
    Encoder( uint8_t, uint8_t, uint8_t );
    
    void begin() ;
    
/****** debounces inputs, to be called with a fixed interval *****/
    void update() ;
    
/**** returns 6 possible states:******
 * rightPulse
 * leftPulse
 * HIGH
 * LOW
 * RISING
 * FALLING
/*****************************/
    uint8_t getState() ;
    
private:
    Debounce clk ;
    Debounce data ;
    Debounce sw ;
    
    uint8_t swPin ;
    uint8_t clkPin ;
    uint8_t dataPin ;
} ;

#endif
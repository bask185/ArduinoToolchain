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
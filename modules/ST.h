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

#ifndef ST_H
#define ST_H

#include <Arduino.h>


const int TIMER_ON     = 0 ;
const int TIMER_OFF    = 1 ;
const int TIMER_BLEEP  = 2 ;
const int TIMER_PULSE  = 3 ;
const int TIMER_TOGGLE = 4 ;

class Timer
{
public:
    Timer();
    void    set( uint8_t _type, uint32_t _PT ) ;
    uint8_t update( uint8_t _IN ) ;
    uint8_t update( ) ;

    uint32_t    PT ;
    uint32_t    ET ;
    uint32_t    startTime;
    
    uint8_t     type         : 3 ;    
    uint8_t     startTrigger : 1 ;
    uint8_t     endTrigger   : 1 ;
    uint8_t     Q            : 1 ;
    uint8_t     IN           : 1 ;
};



/* TRIGGERS */

class Trigger
{
  public:
    Trigger();

    void update( uint8_t input );

    bool rose();
    bool fell();
    bool toggled(); 
    void arm() ;

    uint8_t state    : 1;
    uint8_t armed    : 1;
    uint8_t _toggled : 1;
    uint8_t _rose    : 1;
    uint8_t _fell    : 1;
};

/*****  FLIP FLOPS ******/
class SR
{
public:
    SR() ;
    void update( uint8_t S, uint8_t R ) ;
    uint8_t Q : 1 ;
} ;


class RS
{
public:
    RS() ;
    void update( uint8_t S, uint8_t R ) ;
    uint8_t Q : 1 ;
} ;

class MEM 
{
public:
    MEM() ;
    void update( uint8_t IN ) ;
    uint8_t Q : 1 ;
} ;

/***** COUNTER ****/
class BaseCounter
{
protected:
    uint16_t  counter ;
    
public:
    uint8_t       Q : 1;
    BaseCounter() ;
    uint16_t  getCount() ;
} ;

class UP_COUNTER : public BaseCounter
{
public:
    UP_COUNTER() {}
    uint8_t count( uint8_t IN, uint16_t target ) ;
} ;

class DOWN_COUNTER : public BaseCounter
{
public:
    DOWN_COUNTER() {}
    uint8_t count( uint8_t IN, uint16_t target ) ;
} ;

/******* RAMP GENERATOR *******/
class RAMP_GEN
{
public:
    RAMP_GEN() ;
    void update() ;

    uint16_t setpoint  ;
    uint16_t Y ;
    uint16_t interval ;
    uint8_t  Q : 1 ;
    uint32_t lastStep ;
    uint8_t  EN ;
} ;


#endif

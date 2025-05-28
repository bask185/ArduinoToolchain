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

class BaseTimer 
{
public:
    uint32_t    PT ;
    uint32_t    ET ;
    bool        Q ;

    BaseTimer() ;
    virtual uint8_t update(uint8_t IN) = 0 ;

protected:
    uint32_t    startTime ;
    bool        startTrigger ;
    bool        endTrigger ;
} ;


class TIMER_ON : public BaseTimer 
{
public:
    TIMER_ON() {}
    uint8_t update(uint8_t IN) override;
};


class TIMER_OFF : public BaseTimer 
{
public:
    TIMER_OFF() {}
    uint8_t update(uint8_t IN) override;
};


class TIMER_BLEEP : public BaseTimer 
{
public:
    TIMER_BLEEP() {}
    uint8_t update(uint8_t IN) override;
};


class TIMER_PULSE : public BaseTimer 
{
public:
    TIMER_PULSE() {}
    uint8_t update(uint8_t IN) override;
};



/* TRIGGERS */
class R_TRIG
{
public:
    R_TRIG( ) ;
    void  update( uint8_t ) ;
    uint8_t   Q : 1 ;
    uint8_t old : 1 ;
} ;

class F_TRIG
{
public:
    F_TRIG( ) ;
    void  update( uint8_t )  ;
    uint8_t   Q : 1 ;
    uint8_t old : 1 ;
} ;

class C_trigger
{
public:
    C_trigger() ;
    void update( uint8_t IN ) ;
    uint8_t Q : 1 ;
    uint8_t old : 1 ;
} ;

class T_trigger
{
public:
    T_trigger() ;
    void update( uint8_t IN ) ;
    uint8_t Q : 1 ;
    uint8_t old : 1 ;
} ;

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

/***** COUNTER ****/
class BaseCounter
{
protected:
    uint16_t  counter ;
    
public:
    uint8_t       Q : 1;
    BaseCounter() ;
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

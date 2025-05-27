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

#include "ST.h"


/*   TIMERS   */

BaseTimer::BaseTimer()
{
    Q = 0 ;
    PT = 0 ;
    ET = 0 ;
}


uint8_t TIMER_ON::update(uint8_t IN) 
{
    Q = 0 ;
    if( !IN )
    {
        ET = 0 ;
        startTrigger = 1 ;
        endTrigger   = 1 ;
    }
    else if( startTrigger )
    {
        startTrigger = 0 ;
        startTime = millis() ;
    }

    if( IN )
    {
        ET = millis() - startTime ;
    }

    if( IN && ET >= PT && endTrigger == 1 )
    {
        endTrigger = 0 ;
        Q = 1 ;
    }

    return Q ;
}


uint8_t TIMER_OFF::update(uint8_t IN) 
{
    Q = 0 ;
    if( IN )
    {
        ET = 0 ;
        startTrigger = 1 ;
        endTrigger   = 1 ;
    }
    else if( startTrigger )
    {
        startTrigger = 0 ;
        startTime = millis() ;
    }

    if( !IN )
    {
        ET = millis() - startTime ;
    }

    if( !IN && ET >= PT && endTrigger == 1 )
    {
        endTrigger = 0 ;
        Q = 1 ;
    }

    return Q ;
}


uint8_t TIMER_BLEEP::update(uint8_t IN) 
{
    if( !IN )
    {
        startTime = millis() ;
        ET = 0 ;
    }

    if( IN )
    {
        ET = millis() - startTime ;
        if( ET >= PT )
        {
            startTime = millis() ;
            ET = 0 ;
            Q = 1 ;
        }
        else
        {
            Q = 0 ;
        }
    }
    else
    {
        Q = 0 ;
    }

    return Q ;
}


uint8_t TIMER_PULSE::update(uint8_t IN) 
{
    if( !IN )
    {
        startTime = millis() ;
        endTrigger = 1 ;
        ET = 0 ;
    }

    if( endTrigger == 1 )
    {
        ET = millis() - startTime ;
        if( ET >= PT )
        {
            endTrigger = 0 ;
        }
    }
    else
    {
        ET = 0 ;
    }

    if( IN && endTrigger )
    {
        Q = 1 ;
    }
    else
    {
        Q = 0 ;
    }

    return Q ;
}


/*   TRIGGERS   */
R_TRIG::R_TRIG()
{
}

void R_TRIG::update( uint8_t IN )
{
    Q = 0 ;

    if( IN != old )
    {
        Q   = IN ;
        old = IN ;
    }
} 

F_TRIG::F_TRIG()
{
}

void F_TRIG::update( uint8_t IN )
{
    Q = 0 ;

    if( IN != old )
    {
        Q   = IN ^ 1 ;
        old = IN ;
    }
}

C_trigger::C_trigger()
{
    Q   = 0 ;
    old = 0 ;
}

void C_trigger::update( uint8_t IN )
{
    Q = 0 ;
    if( IN != old )
    {
        Q = 1 ;
        old = IN ;
    }
}

T_trigger::T_trigger()
{
    Q   = 0 ;
    old = 0 ;
}

void T_trigger::update( uint8_t IN )
{
    if( IN == 1 && old == 0 )
    {
        Q ^= 1 ;
    }
    old = IN ;
}


/***** FLIP FLOPS *****/
SR::SR()
{
    Q = 0 ;
}

void SR::update( uint8_t S, uint8_t R )
{
    if( R )       Q = 0 ;
    else if( S )  Q = 1 ;
}


RS::RS()
{
    Q = 0 ;
}

void RS::update( uint8_t S, uint8_t R )
{
    if( S )       Q = 1 ;
    else if( R )  Q = 0 ;
}


/***** COUNTER ****/
void COUNTER::update( uint8_t IN )
{
    Q = 0 ;

    if( IN == 1 && old == 0 )
    {
        if( CV < 0xFFFF ) CV++ ;

        if( CV >= PV )
        {
            Q = 1 ;
        }
    }

    old = IN ;
}

COUNTER::COUNTER()
{
    Q   = 0 ;
    CV  = 0 ;
    PV  = 0 ;
    old = 0 ;
}

void COUNTER::reset()
{
    CV = 0 ;
    Q  = 0 ;
}


/******* RAMP GENERATOR *******/
RAMP_GEN::RAMP_GEN()
{
    SP        = 0 ;
    EN        = 0 ;
    Y         = 0 ;
    interval  = 0 ;
    Q         = 0 ;
    lastStep  = millis() ;
}

void RAMP_GEN::update()
{
    if( !EN ) return ;

    uint32_t now = millis();
    if( now - lastStep < interval ) return ;

    lastStep = now ;

    if( Y < setpoint  ) Y++ ;
    if( Y > setpoint  ) Y-- ;

    Q = ( Y == setpoint  ) ;
}

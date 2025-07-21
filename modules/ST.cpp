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


Timer::Timer()
{
    Q = 0;
    PT = 0;
    ET = 0;
    IN = 0 ;
    startTime = 0;
    startTrigger = true;
    endTrigger = true;
    type = TIMER_ON;
}

void Timer::set( uint8_t _type, uint32_t _PT )
{   
    type = _type ;
    PT   =   _PT ;
    IN   =     1 ;
}


uint8_t Timer::update( uint8_t _IN )
{
    IN = _IN ;
    return update() ;
}

uint8_t Timer::update()
{
    uint32_t now = millis();

    switch (type)
    {
    case TIMER_ON:
    case TIMER_OFF:
    {
        Q = 0 ;
        bool resetCondition = (type == TIMER_ON && !IN) || (type == TIMER_OFF && IN);
        bool activeInput    = (type == TIMER_ON) ? IN : !IN;

        if (resetCondition)
        {
            ET = 0;
            startTrigger = true;
            endTrigger = true;
        }
        else if (startTrigger)
        {
            startTrigger = false;
            startTime = now;
        }

        if (activeInput)
        {
            ET = now - startTime;
        }

        if (activeInput && ET >= PT && endTrigger)
        {
            endTrigger = false;
            Q = 1;
        }
        break;
    }

    case TIMER_BLEEP:
        if (!IN)
        {
            startTime = now;
            ET = 0;
            Q = 0;
        }
        else
        {
            ET = now - startTime;
            if (ET >= PT)
            {
                startTime = now;
                ET = 0;
                Q = 1;
            }
            else
            {
                Q = 0;
            }
        }
        break;

    case TIMER_PULSE:
        if (!IN)
        {
            startTime = now;
            endTrigger = true;
            ET = 0;
        }

        if (endTrigger)
        {
            ET = now - startTime;
            if (ET >= PT)
            {
                endTrigger = false;
            }
        }
        else
        {
            ET = 0;
        }

        Q = (IN && endTrigger) ? 1 : 0;
        break;

    case TIMER_TOGGLE:
        if (!IN)
        {
            startTrigger = true;
            startTime = now;
            ET = 0;
        }
        else
        {
            if (startTrigger)
            {
                startTrigger = false;
                startTime = now;
                ET = 0;
            }

            ET = now - startTime;

            if (ET >= PT)
            {
                Q = !Q;
                startTime = now;
                ET = 0;
            }
        }
        break;

    default:
        Q = 0;
        break;
    }

    return Q;
}


/*   TRIGGERS   */

Trigger::Trigger ()
{
    _toggled = 0 ; 
    _rose    = 0 ; 
    _fell    = 0 ; 
    state    = 0 ; 
    armed    = 0 ;
}

void Trigger::update( uint8_t IN )
{
    _toggled = 0 ;
    _rose    = 0 ;
    _fell    = 0 ;

    if( IN != state )
    {
        state = IN ;

        if ( !armed )
        {
            armed = 1 ;
            return ;
        }

        _toggled = 1 ;
        if ( IN ) _rose = 1 ;
        else      _fell = 1 ;
    }
}

void Trigger::arm()
{
    armed = 1 ;
}

bool Trigger::toggled()
{
    if ( _toggled )
    {
        _toggled = 0;
        return 1;
    }
    return 0;
}

bool Trigger::rose()
{
    if ( _rose )
    {
        _rose = 0;
        return 1;
    }
    return 0;
}

bool Trigger::fell()
{
    if ( _fell )
    {
        _fell = 0;
        return 1;
    }
    return 0;
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

MEM::MEM()
{
}

void MEM::update( uint8_t IN )
{
    if( IN ) Q = 1 ;
    else     Q = 0 ;
}

/***** COUNTER ****/
BaseCounter::BaseCounter() {}

uint16_t BaseCounter::getCount()
{
    return counter ;
}

uint8_t UP_COUNTER::count( uint8_t IN, uint16_t target )
{
    Q = 0 ;

    if( !IN )
    {
        counter = 0 ;
    }

    else
    {
        if( counter  < target ) counter ++ ;
        if( counter == target ) Q = 1 ;
    }

    return Q ;
}


uint8_t DOWN_COUNTER::count( uint8_t IN, uint16_t startPoint )
{
    Q = 0 ;

    if( !IN )
    {
        counter = startPoint ;
    }

    else
    {
        if( counter  > 0 ) counter -- ;
        if( counter == 0 ) Q = 1 ;
    }
    
    return Q ;
}

/******* RAMP GENERATOR *******/
RAMP_GEN::RAMP_GEN()
{
    setpoint  = 0 ;
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

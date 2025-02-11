#include "Timers.h"

BaseTimer::BaseTimer()
{
    startTrigger = 1 ;
    endTrigger   = 1 ;
}


void BaseTimer::setTime( uint32_t time )
{
    presetTime = time ;
}


uint8_t TIMER_ON::update(uint8_t IN) 
{
    Q = 0 ;
    if( !IN )
    {
        startTrigger = 1 ;
        endTrigger   = 1 ;
    }
    else if( startTrigger )
    {
        startTrigger = 0 ;
        startTime = millis() ;
    }

    if( IN 
    &&  millis() - startTime >= presetTime 
    &&  endTrigger == 1  )
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
        startTrigger = 1 ;
        endTrigger   = 1 ;
    }
    else if( startTrigger )
    {
        startTrigger = 0 ;
        startTime = millis() ;
    }

    if( !IN
    &&  millis() - startTime >= presetTime 
    &&  endTrigger == 1 )
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
    }

    if( IN && (millis() - startTime) >= presetTime )
    {
        startTime = millis() ;
        Q = 1 ;
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
        endTrigger   = 1 ;
    }

    if( endTrigger == 1 && (millis() - startTime) >= presetTime )
    {
        endTrigger = 0 ;
    }

    if( IN && endTrigger ) Q = 1 ; 
    else                   Q = 0 ;

    return Q ;
}

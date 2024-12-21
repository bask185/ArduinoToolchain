#include "Timers.h"

#include <Arduino.h>

uint8_t TON_timer::update(uint8_t IN) 
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

uint8_t TOFF_timer::update(uint8_t IN) 
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

uint8_t BlinkTimer::update(uint8_t IN) 
{
    if( !IN )
    {
        startTime = millis() ;
    }

    if( IN && (millis() - startTime) >= presetTime )
    {
        startTime = millis() ;
        Q = !Q ;
    }
    else
    {
        Q = 0 ;
    }
    return Q ;
}

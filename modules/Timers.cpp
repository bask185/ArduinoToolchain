#include "Timers.h"

// Implementatie van de On-Delay Timer (TON)
TON_timer::TON_timer(uint32_t presetTime)
{
    this->presetTime = presetTime ;
    this->startTime = 0 ;
    this->active = 0 ;
    this->triggered = 0 ;
    this->Q = 0 ;
}

uint8_t TON_timer::update(uint8_t IN)
{
    if (!IN)
    {
        active = 0 ;
        triggered = 0 ;
        Q = 0 ;
        return Q ;
    }

    if (!active)
    {
        startTime = millis() ;
        active = 1 ;
    }

    if (millis() - startTime >= presetTime && !triggered)
    {
        Q = 1 ;
        triggered = 1 ;
    }

    uint8_t result = Q ;
    Q = 0 ;
    return result ;
}

// Implementatie van de Off-Delay Timer (TOFF)
TOFF_timer::TOFF_timer(uint32_t presetTime)
{
    this->presetTime = presetTime ;
    this->startTime = 0 ;
    this->active = 0 ;
    this->triggered = 0 ;
    this->Q = 1 ;
}

uint8_t TOFF_timer::update(uint8_t IN)
{
    if (IN)
    {
        active = 0 ;
        triggered = 0 ;
        Q = 0 ;
        return Q ;
    }

    if (!active)
    {
        startTime = millis() ;
        active = 1 ;
    }

    if (millis() - startTime >= presetTime && !triggered)
    {
        Q = 1 ;
        triggered = 1 ;
    }

    uint8_t result = Q ;
    Q = 0 ;
    return result ;
}

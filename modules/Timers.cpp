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

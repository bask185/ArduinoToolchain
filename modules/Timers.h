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

#ifndef TIMERS_H
#define TIMERS_H

#include <Arduino.h>

// Klasse voor de On-Delay Timer (TON)
class TON_timer
{
public:
    TON_timer(uint32_t presetTime);
    uint8_t update(uint8_t IN);

    uint8_t Q : 1;  // Output

private:
    uint32_t startTime ;
    uint32_t presetTime ;
    uint8_t active : 1 ;
    uint8_t triggered : 1 ;  // Geeft aan of de timer al getriggerd is
};

// Klasse voor de Off-Delay Timer (TOFF)
class TOFF_timer
{
public:
    TOFF_timer(uint32_t presetTime);
    uint8_t update(uint8_t IN);

    uint8_t Q : 1;  // Output

private:
    uint32_t startTime ;
    uint32_t presetTime ;
    uint8_t active : 1 ;
    uint8_t triggered : 1 ;  // Geeft aan of de timer al getriggerd is
};

#endif // TIMERS_H
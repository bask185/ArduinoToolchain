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

#include <Arduino.h>

class LDR {
public:
    LDR( uint8_t _pin ) ;
    uint8_t update ( ) ;
    uint8_t getState ( ) ;

private:
    const uint8_t nSamples = 5 ;
    const uint8_t sampleInterval = 100 ;

    uint8_t     pin ;
    uint8_t     state ;
    uint8_t     sampleCounter ;
    uint16_t    sample[ nSamples ] ;
    uint32_t    previousTime ;
}

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
#include "macros.h"

// CONTROL FUNCTIONS
extern void     setSpeed(        uint16, uint8 ) ;
extern void     setAccessory(    uint16, uint8 ) ;
extern void     setAccessoryExt( uint16, uint8 ) ;
extern void     setFunction( uint16, uint8, uint8 ) ;

extern void initDCC() ;
extern void DCCsignals() ;

extern void dccISR() __attribute__((weak)) ;
extern void oneBit() __attribute__((weak)) ;
extern void zeroBit() __attribute__((weak)) ;
extern void zeroBit2() __attribute__((weak)) ;
extern void endBit() __attribute__((weak)) ;

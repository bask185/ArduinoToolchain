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

extern void shortCircuitInit(uint8_t, uint8_t, uint8*, uint32_t, uint32_t, uint8_t);
// max current is the adc value corresponding with the provided current and resistande
// the calculation:  max adc value = ( 1024 * I * R ) / 5
// we do not work with decimal values, there for we work with milli Amperes and milli Ohms

extern uint8_t shortCircuit();
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

/* this library calculates a desired servo position for a turnout and toggles a relay when servo\
is halfway. The update function returns a servo position when a servo is transitioning. When a 
servo reached one of the 2 end positions, the value 0 will be returned

The library does not directly control a servo, it merely calculates what it's position should be.
This allows user to use whatever library or controll symstem he or she preffers.

*/

#define servoMin 45
#define servoMax 135
#define middlePos 90
#define straight 0
#define curved 1

Class Turnout {
public:
    Turnout( uint8_t _pin ) ;
    setTurnout( uint8_t _state ) ;
    uint8_t update() ;

private:
    uint8_t state ;
    uint8_t relayPin ;
    uint8_t servoPos ;
}
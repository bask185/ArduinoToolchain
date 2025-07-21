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

#include "weistra.h"


Weistra::Weistra(uint8_t _pin, uint8_t _Fmin, uint8_t _Fmax ) 
{ // constructor
    trackPin1 = _pin;
    trackPin2 = 255 ;
    Fmin = _Fmin ;
    Fmax = _Fmax ;
    sensePin = 255 ;
}

Weistra::Weistra(uint8_t _pin1, uint8_t _pin2, uint8_t _Fmin, uint8_t _Fmax ) 
{ // constructor
    trackPin1 = _pin1;
    trackPin2 = _pin2;
    Fmin = _Fmin ;
    Fmax = _Fmax ;
    sensePin = 255 ;
} 


void Weistra::begin()
{
    pinMode(trackPin1, OUTPUT);
    pinMode(trackPin2, OUTPUT);

    uint8_t port1 = digitalPinToPort( trackPin1 );
    trackPin1     = digitalPinToBitMask( trackPin1 );
    portx_p1      = portOutputRegister( port1 );

    if( trackPin2 != 255 )
    {
        uint8_t port2 = digitalPinToPort( trackPin2 );
        trackPin2     = digitalPinToBitMask( trackPin2 );
        portx_p2      = portOutputRegister( port2 );
    }

    state = 1 ;
}

/*currentLimit = Imax * Rshunt / 0.00488 */
void Weistra::setCurrentSense( uint8_t _sensePin, uint8_t _currentLimit )
{
    sensePin = _sensePin ;
    currentLimit = _currentLimit ;
}

void Weistra::update() 
{
    if( portx_p1 || portx_p2 )
    {
        uint32_t currentTime = micros() ; 

        if( currentTime - prevTime >= intervalTime )
        {   prevTime = currentTime;

            if( !state )
            {
                *portx_p1 &= ~trackPin1;
                *portx_p2 &= ~trackPin2;
            }
            else if( counter == 0 && newDutyCycle > 0 )      // if counter reaches 100, reset it to 0 and enable the track power pin
            {
                if(dir) *portx_p1 |=  trackPin1 ;
                else    *portx_p2 |=  trackPin2 ;

                dutyCycle = newDutyCycle ;              // a new dutycucle can only be accepted on the beginning of a cycle, this prevents weird jumps of the trains
                intervalTime = newIntervalTime ;        // new speed is accepted at the beginning of a cycle
            }
            if( counter == dutyCycle /*&& dutyCycle < 100*/ ) // commented code seems buggy??
            {
                *portx_p1 &= ~trackPin1;
                *portx_p2 &= ~trackPin2;
            }
            if( ++counter > 100) counter = 0 ;
        }

        if( sensePin != 255 )
        {
            int current = analogRead( sensePin ) ;

            if( millis() - prevTime2 > interval 
            && (*portx_p1 & trackPin1 || *portx_p2 & trackPin2 || !state ) )
            {     prevTime2 = millis() ;

                if( state ) // if on
                {
                    if( current > currentLimit ) currentCounter -- ;
                    else                         currentCounter = 5 ;

                    if( currentCounter == 0 )
                    {
                        setState( 0 ) ;
                        interval = 5000 ; // 5s power off
                    }
                }
                else        // if off
                {
                    setState( 1 ) ; // turn on again
                    interval = sampleRate ; // set sample rate 
                    currentCounter = 2 ;
                }
            }
        }
    }
}


void Weistra::setSpeed( int8_t speed)
{
    byte frequency;
    speedBackup = speed ;
    if( speed < 0 ) dir = 1 ; // 1 is reverse
    if( speed > 0 ) dir = 0 ;

    speed = abs( speed ) ;

    newDutyCycle = constrain( speed, 0, 100 ) ; // speed limit = 0 - 100

    if( newDutyCycle <= 10 ) { frequency = Fmin; }
    else                     { frequency = map( newDutyCycle, 10, 100, Fmin, Fmax ) ; }

    newIntervalTime = 10000 / frequency; // > between 100us and 500us
}

int8_t Weistra::getSpeed()
{
    return speedBackup ;
}

void Weistra::setState( uint8_t _state )
{ 
    state = _state ;
    
    if( state )
    {
        counter = 0 ;
    }
    else
    {
        *portx_p1 &= ~trackPin1 ;
        *portx_p2 &= ~trackPin2 ;
    }
}

uint8_t Weistra::getState( ) 
{
    return state ;
}



void updateTrackPower()
{
    uint32_t currentTime = micros() ; 

    if( currentTime - prevTime >= 200 )     // 50 HZ
    {   prevTime = currentTime;

        if( !state )
        {
            PORTB = 0x00 ;                              // turn off all track pins at once
            PORTD = 0x00 ;
        }
        else if( counter == 0 && newDutyCycle > 0 )    // if counter reaches 100, reset it to 0 and enable the track power pin, begin of cycle
        {
            PORTB = 0xFF & track_mask_1 ;              // turn on ALL track at the same time.
            PORTD = 0xFF & track_mask_2 ;              // With an AND mask you can pick which tracks go ON and which go OFF
            // etc 

            dutyCycle[0] = newDutyCycle[0] ; 
            dutyCycle[1] = newDutyCycle[0] ; 
            dutyCycle[0] = newDutyCycle[0] ; 
        }
        if( counter == dutyCycle[0] )  PORTB &= ~TRACK_1_MASK ; // 8 bit comparison are somewhat fast, followed by direct port manipulation
        if( counter == dutyCycle[1] )  PORTB &= ~TRACK_2_MASK ;
        if( counter == dutyCycle[2] )  PORTB &= ~TRACK_3_MASK ;
        if( counter == dutyCycle[3] )  PORTB &= ~TRACK_4_MASK ;
        if( counter == dutyCycle[4] )  PORTB &= ~TRACK_5_MASK ;
        if( counter == dutyCycle[5] )  PORTB &= ~TRACK_6_MASK ;

        if( counter == dutyCycle[6] )  PORTD &= ~TRACK_7_MASK ;
        if( counter == dutyCycle[7] )  PORTD &= ~TRACK_8_MASK ;
        if( counter == dutyCycle[8] )  PORTD &= ~TRACK_9_MASK ;
        if( counter == dutyCycle[9] )  PORTD &= ~TRACK_10_MASK ;
        if( counter == dutyCycle[10] ) PORTD &= ~TRACK_11_MASK ;
        if( counter == dutyCycle[11] ) PORTD &= ~TRACK_12_MASK ;

        
        if( ++counter > 100) counter = 0 ;  // 100 speed steps
    }
}
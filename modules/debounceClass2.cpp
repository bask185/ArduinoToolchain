#include "debounceClass2.h"

Debouncer::Debouncer()              // no digital read, default interval
{
    pin = 255 ;
}

Debouncer::Debouncer( uint8 _pin )   // with digital read default interval
{
    pin = _pin ;
    pinMode( pin, INPUT_PULLUP ) ;
}

Debouncer::Debouncer( uint8 _pin, uint32 _interval )  // with digital read and custom interval
{
    pin      = _pin ;
    pinMode( pin, INPUT_PULLUP ) ;
    prevTime = millis() - _interval; // Initialize prevTime based on the custom interval
}

Debouncer::Debouncer( uint32 _interval )
{
    pin      = 255 ;
    prevTime = millis() - _interval; // Initialize prevTime based on the custom interval
}

void Debouncer::debounce( uint8 newSample )
{
    if( millis() - prevTime >= DEFAULT_INTERVAL )   // gebruik de compile-time constante voor het interval
    {    
        prevTime = millis() ;

        if( newSample == oldSample )   // if the same state is detected at least twice in 20ms...
        {
            if(newSample != statePrev) // if a flank change occured, return RISING or FALLING
            {
                statePrev = newSample ;

                if( newSample ) newState =  RISING ; 
                else      newState = FALLING ;
            }
            else            // or if there is no flank change return PRESSED or RELEASED
            {
                if(newSample) newState = HIGH ; 
                else      newState =  LOW ;
            }
        }

        oldSample = newSample;
    }

    state = newState ;

    if(newState ==  RISING ) newState = HIGH ;
    if(newState == FALLING ) newState =  LOW ;
}

void Debouncer::debounce()
{
    debounce( digitalRead( pin ) ) ; // use digital read to feed the debouncer.
}

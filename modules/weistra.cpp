#include "weistra.h"


Weistra::Weistra(uint8_t _pin, uint8_t _Fmin, uint8_t _Fmax ) 
{ // constructor
    trackPin = _pin;
    Fmin = _Fmin ;
    Fmax = _Fmax ;
} 

void Weistra::begin()
{
    pinMode(trackPin, OUTPUT);

    byte port   = digitalPinToPort( trackPin );
    trackPin    = digitalPinToBitMask( trackPin );
    portx_p     = portOutputRegister( port );
}

void Weistra::update() 
{
    if( portx_p != 0 )
    {
        uint32_t currentTime = micros() ; 

        if( currentTime - prevTime >= intervalTime )
        { 
            prevTime = currentTime;

            if( counter == 0 && newDutyCycle > 0 )      // if counter reaches 100, reset it to 0 and enable the track power pin
            {
                *portx_p |=  trackPin ;
                dutyCycle = newDutyCycle ;              // a new dutycucle can only be accepted on the beginning of a cycle, this prevents weird jumps of the trains
                intervalTime = newIntervalTime ;        // new speed is accepted at the beginning of a cycle
            }
            if( counter == dutyCycle /*&& dutyCycle < 100*/ ) // commented code seems buggy??
            {
                *portx_p &= ~trackPin;
            }
            if( ++counter > 100) counter = 0 ;
        }
    }
}


void Weistra::setSpeed(byte speed)
{
    byte frequency;

    newDutyCycle = constrain( speed, 0, 100 ) ; // speed limit = 0 - 100

    if( newDutyCycle <= 10 ) { frequency = Fmin; }
    else                     { frequency = map( newDutyCycle, 10, 100, Fmin, Fmax ) ; }

    newIntervalTime = 10000 / frequency; // > between 100us and 500us
}

void Weistra::setState( uint8_t _state )
{ 
    state = _state ;
    
    if( !state )
    {
        *portx_p &= ~trackPin ;
    }
    else
    {
        counter = 0 ;
    }
}

uint8_t Weistra::getState( ) 
{
    return state ;
}


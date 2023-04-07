#include "CoilDrive.h"

CoilDrive::CoilDrive( uint8 _pinA, uint8 _pinB, uint32 _switchTime )
{
    pinA = _pinA ;
    pinB = _pinB ;
    switchTime = _switchTime ;
}

void CoilDrive::begin()
{
    pinMode( pinA, OUTPUT ) ;
    pinMode( pinB, OUTPUT ) ;
}

void CoilDrive::setState( uint8 _state )
{
    prevTime = millis() ;
    set = true ;

    if( _state ) digitalWrite( pinA, HIGH ) ;
    else         digitalWrite( pinB, HIGH ) ;
}

void CoilDrive::update()
{
    if( set == true && (millis() - prevTime) >= switchTime )
    {   set  = false ;
        
        digitalWrite( pinA, LOW ) ;
        digitalWrite( pinB, LOW ) ;
    }
}
#include "LedBlink.h"

LedBlink::LedBlink( uint8_t _pin ) 
{
    pin = _pin ;
}

void LedBlink::begin()
{
    pinMode( pin, OUTPUT ) ;
    digitalWrite( pin, LOW );
    Serial.println("beginning led");
}

void LedBlink::bleep( uint8_t _count )
{
    eventBleeps = _count  ;
    mode = BLEEPING ;
    // eventActive = false ;
    // cycleCounter = 0 ;
    // lastTime = millis() ;
}

void LedBlink::turn( uint8_t on_off )
{
    mode = on_off ;
}

void LedBlink::blink(uint8_t F)
{
    mode = F ;
}


void LedBlink::setEventBleeps( uint8_t _count )
{
    eventBleeps = _count + 1 ; // I dont really get why I had to add this + 1 though..
    eventActive = true ;
    cycleCounter = 0 ;
    lastTime = millis() ;
}


void LedBlink::update()
{
    if( millis() - lastTime >= interval )
    {       lastTime  = millis() ;

        if( ++ cycleCounter == nCycles ) { cycleCounter = 0 ; eventActive = 0 ; }

 
        if( eventActive )
        {
            if (eventBleeps >=(cycleCounter / 2) + 1 
            && cycleCounter  % 2 == 0)
            {                
                digitalWrite( pin, HIGH ) ;
            }
            else
            {
                digitalWrite( pin, LOW ) ;  
            }
        }
        else
        {
            switch( mode )
            {
            case OFF:
                digitalWrite( pin, LOW ) ; 
                break ;

            case ON:
                digitalWrite( pin, HIGH ) ;
                break ;

            case BLEEPING:
                if (eventBleeps >= (cycleCounter / 2) + 1 
                &&  cycleCounter % 2 == 0)
                {                
                    digitalWrite( pin, HIGH ) ;
                }
                else
                {
                    digitalWrite( pin, LOW ) ;  
                }
                break ;

            case SLOW:
                if( (cycleCounter % 5) == 0 )
                {
                    digitalWrite( pin, ledState ) ;
                    ledState ^= 1 ;
                }
                break ;

            case FAST:
                if( (cycleCounter % 2) == 0 )
                {
                    digitalWrite( pin, ledState ) ;
                    ledState ^= 1 ;
                }
                break ;

            case REALLY_FAST:
                if( (cycleCounter % 1) == 0 )
                {
                    digitalWrite( pin, ledState ) ;
                    ledState ^= 1 ;
                }
                break ;
            }
        }
    }
}
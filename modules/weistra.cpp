#include "weistra.h"


Weistra::Weistra(uint8_t _pin, uint8_t _Fmin, uint8_t _Fmax )
{ // constructor
    trackPin = _pin;
    Fmin = _Fmin ;
    Fmax = _Fmax ;
    currentSensePin = 0xff ;        // flag that current sensing is not on
}

Weistra::Weistra(uint8_t _pin, uint8_t _Fmin, uint8_t _Fmax, uint8_t _currentSensePin, uint16_t _maxCurrent )
{ // constructor 2
    trackPin = _pin;
    Fmin = _Fmin ;
    Fmax = _Fmax ;
    currentSensePin = _currentSensePin ;
    maxCurrent = _maxCurrent ;
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
    if( portx_p == 0 ) return ;

    uint32_t currentTime = millis() ; 

    if( currentSensePin != 0xFF )                                       // if this option is turned on, go measure current
    {
        if( currentTime - shortCircuitTimer >= shortCircuitInterval )   // if time has expired for new sample
        {
            if( power == 0 ) 
            {
                shortCircuitInterval = 5 ;                              // set interval time back to 5ms
                power = 1 ;                                             // turn power back on
                return ;                                                // and return
            }

            if( ((*portx_p) & trackPin) == 0 ) return ;                 // if track pin is OFF, we cannot measure overcurrent, thus return.

            shortCircuitTimer = currentTime ;                           // otherwise, store previous time for next interfal

            int sample = analogRead( currentSensePin ) ;

            if( sample < _maxCurrent ) { counter = 11 ; }               // if no over current, keep setting the counter to 11
            if( counter ) counter -- ;
            if( counter == 0 )                                          // counter always decrements, if it reaches 0, there must be a short circuit
            {
                power = 0 ;
                *portx_p &= ~trackPin;                                  // kill track power
                shortCircuitInterval = 5000 ;                           // 5 second wait time before power is reapplied
            }
        }
    }

    currentTime = micros() ; 

    if( currentTime - prevTime >= intervalTime )
    {
        prevTime = currentTime;

        if( counter == 0 && dutyCycle > 0)
        {
            *portx_p |=  trackPin;
            intervalTime = newIntervalTime ;
        }
        if( counter == dutyCycle && dutyCycle < 100)
        {
            *portx_p &= ~trackPin;
        }

        if( ++counter > 100) counter = 0;
    }
}



void Weistra::setSpeed(byte speed) {
    byte frequency;

    dutyCycle = constrain(speed, 0, 100); // speed limit = 0 - 100

    if( dutyCycle <= 10 ) { frequency = Fmin; }
    else                  { frequency = map(dutyCycle, 10, 100, Fmin, Fmax); }

    newIntervalTime = 10000 / frequency; // > between 100us and 500us
}

void Weistra::setState( uint8_t _state )
{ 
    power = _state ;
    
    if( !state )
    {
        *portx_p &= ~trackPin;
    }
    else
    {
        counter = 0 ;
    }
}

uint8_t Weistra::getState( ) 
{
    return power ;
}


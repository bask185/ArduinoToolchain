#ifndef DEBOUNCECLASS2_H
#define DEBOUNCECLASS2_H

#include <Arduino.h>
#include "macros.h"

class Debouncer
{
public:
    static constexpr uint32 DEFAULT_INTERVAL = 20;  // Voeg een compile-time constante toe voor het default interval

    Debouncer() ;                                   // default debounce interval van 20ms
    Debouncer( uint8 ) ;                            // default debounce interval van 20ms
    Debouncer( uint8, uint32 ) ;    
    Debouncer( uint32 ) ;

    void debounce() ;
    void debounce( uint8 ) ;

    uint8     state : 2 ;
    uint8 oldSample : 1 ;
    uint8       pin : 6 ;
    uint8 statePrev : 1 ;
    uint8  newState : 2 ;

private:
    uint32 prevTime ;
} ;

#endif

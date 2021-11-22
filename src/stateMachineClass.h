#include <Arduino.h>
#include "macros.h"

#define StateFunction( x ) bool x##F()
#ifndef DEBGUG
#define State(x) break; case x: if(x##F())
#else   
#define State(x) break; case x: if(sm.runOnce) Serial.println(#x); if(x##F())   // if debug is defined, all states are stringerized and printed when entry state is run
#endif
#define STATE_MACHINE_BEGIN if( sm.run() ) switch( sm.getState() ) {
#define STATE_MACHINE_END break ; } return sm.getState() ;

class StateMachine {
public:
    StateMachine() ;
    
    void    setState( uint8_t ) ;
    uint8_t getState() ;
    void    nextState( uint8_t, uint32_t ) ;
    uint8_t entryState() ;
    uint8_t onState() ;
    uint8_t exitState() ;
    uint8_t run() ;
    
    void    setTimeout( uint32_t ) ;
    uint8_t timeout() ;
    void    exit() ;
    void    reboot() ;
    uint8_t endState() ;
    
private:
    uint8_t  runOnce ;
    uint8_t  enabled ;
    uint8_t  exitFlag ;
    uint32_t prevTime ;
    uint32_t interval ;
    uint8_t  state;
} ;



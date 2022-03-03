#include <Arduino.h>
#include "macros.h"

#define StateFunction( x ) bool x##F()
#ifndef DEBUG
#define State(x) break; case x: if(x##F())
#else   
#define State(x) break; case x: if(sm.runOnce) Serial.println(#x); if(x##F())   // if debug is defined, all states are stringerized and printed when entry state is run
#endif
#define STATE_MACHINE_BEGIN(x) if( x.run() ) switch( x.getState() ) {
#define STATE_MACHINE_END(x) break ; } return x.getState() ;

class StateMachine {
public:
    StateMachine() ;
    
    void  setState( uint8 ) ;
    uint8 getState() ;
    void  nextState( uint8, uint32 ) ;
    uint8 entryState() ;
    uint8 onState() ;
    uint8 exitState() ;
    uint8 run() ;
    void  setTimeout( uint32 ) ;
    uint8 timeout() ;
    uint8 timeoutError() ;

    void  exit() ;
    void  reboot( uint32 ) ;
    uint8 endState() ;
    uint8 repeat( uint32 ) ;
    #ifdef DEBUG
    uint8  runOnce ;  // if debug is active, this must be public in order to print the state names
    #endif
    
private:
    #ifndef DEBUG
    uint8  runOnce ;  // is usually private
    #endif
    uint8  enabled ;
    uint8  exitFlag ;
    uint32_t prevTime ;
    uint32_t interval ;
    uint8  state;
    uint8  timeOutSet ;
} ;



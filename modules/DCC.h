#include <Arduino.h>
#include "macros.h"

// CONTROL FUNCTIONS
extern void     setSpeed(        uint16, uint8 ) ;
extern void     setAccessory(    uint16, uint8 ) ;
extern void     setAccessoryExt( uint16, uint8 ) ;
extern void     setFunction( uint16, uint8, uint8 ) ;

extern void initDCC() ;
extern void DCCsignals() ;

extern void dccISR() __attribute__((weak)) ;
extern void oneBit() __attribute__((weak)) ;
extern void zeroBit() __attribute__((weak)) ;
extern void zeroBit2() __attribute__((weak)) ;
extern void endBit() __attribute__((weak)) ;

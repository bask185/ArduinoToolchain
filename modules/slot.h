#include "macros.h"
#include <Arduino.h>


const int NA              = 0xFF ;
const int maxSlot         =   25 ; // max amount of active trains.

const int newSpeed        = 0x01 ;
const int new_F0_F4       = 0x02 ;
const int new_F5_F8       = 0x04 ;
const int new_F9_F12      = 0x08 ; 
const int new_F13_F20     = 0x10 ;
const int new_F21_F28     = 0x20 ;
const int new_F29_F36     = 0x40 ;
// const int new_  = 0x80 ; future reserved function.

const int nPacketTypes  =    7 ; // for locomotives

typedef struct Slots
{
    uint8       speed ;
    uint16    address ;
    uint8     newInstruction ;            
    uint8       F0_F4 ;
    uint8       F5_F8 ;
    uint8      F9_F12 ;
    uint8     F13_F20 ;
    uint8     F21_F28 ;
    uint8     F29_F36 ;
    uint8     priority : 7 ;
    uint8          set : 1 ;
} Slot ;

extern Slot     slot[maxSlot]  ;

// extern void     commissionSlot( uint8 , uint16  ) ; perhaps needed for loconet interface, keep in comment until then
extern uint8    getSlot( uint16 ) ;
extern void     updateSlots() ;


// extern void     initSlots() ;
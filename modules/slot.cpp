#include "slot.h"

Slot slot[ maxSlot ]  ;

void updateSlots()
{
    REPEAT_MS( 60000 ) // every minute, update all slot's priorities
    {
        for( int i = 0 ; i < maxSlot ; i++ )
        {
            if( slot[i].set )
            {   slot[i].set = 0 ;

                if( slot[i].priority <  40 ) slot[i].priority ++ ;
            }
            else
            {
                if( slot[i].priority >   0 ) slot[i].priority -- ;
            }
        }
    }
    END_REPEAT
}


uint8 getFreeSlot() // if new slot is needed, find a slot with 0 priority and otherwise pick the slot with the lowest priority
{
    uint8 lowestPrio =  40 ;
    uint8 slotToUse  =   0 ;

    for( int i = 0 ; i < maxSlot ; i++ )
    {
        if( slot[i].priority < lowestPrio )
        {
            lowestPrio = slot[i].priority ;
            slotToUse = i ;
        }
        if( lowestPrio == 0 ) break ; // we aren't going to find a lower value and this slot is free, so use it
    }
    printNumberln(F("Slot allocated: "), slotToUse ) ;

    return slotToUse ;
}

// find and initialize a slot for given address
uint8 commissionSlot( uint16 address )
{
    uint8 slotNumber = getFreeSlot() ;  // find either a free or the least used slot to commission
    printNumberln(F("for address: "), address ) ;

    slot[slotNumber].address  = address ; // couple address
    slot[slotNumber].priority = 20 ;      // new trains will be active for atleast 20 minutes if not received new instructions.
    
    return slotNumber ;
}

uint8 getSlot( uint16 address )
{
    for (int i = 0; i < maxSlot; i++)
    {
        if( address == slot[i].address )
        {
            printNumberln(F("Slot found "),i ) ;
            return i ;                  // match found, return slot number
        }
    }

    return commissionSlot( address ) ;  // no slot found -> commission new slot and return it
}

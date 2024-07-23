#include "Trigger.h"

R_trigger::R_trigger()
{
}

void R_trigger::update( uint8 IN )
{
    Q = 0 ;

    if( IN != old )
    {
        Q   = IN ;
        old = IN ;
    }
} 

F_trigger::F_trigger()
{
}

void F_trigger::update( uint8 IN )
{
    Q = 0 ;

    if( IN != old )
    {
        Q   = IN ^ 1 ;
        old = IN ;
    }
} 
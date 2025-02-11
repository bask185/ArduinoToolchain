#include "Counter.h"

BaseCounter::BaseCounter() {}

uint8_t UP_COUNTER::count( uint8_t IN, uint16_t target )
{
    Q = 0 ;

    if( !IN )
    {
        counter = 0 ;
    }

    else
    {
        if( counter  < target ) counter ++ ;
        if( counter == target ) Q = 1 ;
    }

    return Q ;
}


uint8_t DOWN_COUNTER::count( uint8_t IN, uint16_t startPoint )
{
    Q = 0 ;

    if( !IN )
    {
        counter = startPoint ;
    }

    else
    {
        if( counter  > startPoint ) counter -- ;
        if( counter ==          0 ) Q = 1 ;
    }
    
    return Q ;
}
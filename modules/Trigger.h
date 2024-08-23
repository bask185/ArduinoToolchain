#include <Arduino.h>
#include "macros.h"

class R_trigger
{
public:
    R_trigger( ) ;
    void  update( uint8 ) ;
    uint8     Q : 1 ;

private:
    uint8   old : 1 ;
} ;

class F_trigger
{
public:
    F_trigger( ) ;
    void  update( uint8 )  ;
    uint8     Q : 1 ;

private:
    uint8   old : 1 ;
} ;
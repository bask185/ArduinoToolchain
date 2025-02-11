#include <Arduino.h>

class BaseCounter
{
protected:
    uint16_t  counter ;
    
public:
    uint8_t       Q : 1;
    BaseCounter() ;
} ;

class UP_COUNTER : public BaseCounter
{
public:
    UP_COUNTER() {}
    uint8_t count( uint8_t IN, uint16_t target ) ;
} ;

class DOWN_COUNTER : public BaseCounter
{
public:
    DOWN_COUNTER() {}
    uint8_t count( uint8_t IN, uint16_t target ) ;
} ;
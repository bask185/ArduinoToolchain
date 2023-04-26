#include <Arduino.h>
#include "macros.h"

class CoilDrive {

public:
    CoilDrive( uint8_t, uint8_t, uint32 ) ;
    void setState( uint8_t );
    void update() ;
    void begin() ;

private:
    uint32  switchTime ;
    uint32  prevTime ;
    uint8   pinA ;
    uint8   pinB ;
    bool    set ;
} ;
#include <Arduino.h>

class LDR {
public:
    LDR( uint8_t _pin ) ;
    uint8_t update ( ) ;
    uint8_t getState ( ) ;

private:
    const uint8_t nSamples = 5 ;
    const uint8_t sampleInterval = 100 ;

    uint8_t     pin ;
    uint8_t     state ;
    uint8_t     sampleCounter ;
    uint16_t    sample[ nSamples ] ;
    uint32_t    previousTime ;
}

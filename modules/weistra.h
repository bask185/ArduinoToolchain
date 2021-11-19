#include <Arduino.h>

class Weistra {
public:
    Weistra( uint8_t, uint8_t, uint8_t) ;
    Weistra( uint8_t, uint8_t, uint8_t uint8_t ) ;
    void begin() ;
    void update() ;
    void setSpeed(uint8_t) ;
    void setState(uint8_t ) ;
    uint8_t getState( ) ;

private:
    uint16_t intervalTime ;
    uint16_t newIntervalTime ;
    uint8_t trackPin ;
    uint8_t dutyCycle ;
    uint8_t Fmin ;
    uint8_t Fmax ;
    uint8_t power ;
    uint8_t counter = 0;
    uint8_t downCounter ;
    uint32_t prevTime = 0;
    
    volatile uint8_t *portx_p ;

    uint8_t  currentSensePin ;
    uint32_t shortCircuitTimer ;
    uint16_t shortCircuitInterval = 5 ; // 5ms
    uint16_t maxCurrent ;
};


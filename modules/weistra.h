#include <Arduino.h>

class Weistra {
public:
    Weistra(uint8_t, uint8_t, uint8_t);
    Weistra(uint8_t, uint8_t, uint8_t, uint8_t);
    void begin();
    void update();
    void setSpeed(int8_t);
    int8_t getSpeed();
    void setState(uint8_t ) ;
    void setCurrentSense( uint8_t, uint8_t ) ;
    uint8_t getState( ) ;

private:
    uint32_t intervalTime ;
    uint16_t newIntervalTime ;
    uint16_t interval ;
    uint32_t prevTime ;
    uint8_t trackPin1 ;
    uint8_t trackPin2 ;
    uint8_t dutyCycle ;
    uint8_t newDutyCycle ;
    uint8_t Fmin ;
    uint8_t Fmax ;
    uint8_t state ;
    uint8_t dir ;
    uint8_t currentLimit ;
    uint8_t sensePin ;
    int8_t  speedBackup ;
    uint8_t counter = 0;
    uint8_t currentCounter = 0;
    uint32_t prevTime2 = 0;

    const int sampleRate = 10 ;

    volatile uint8_t *portx_p1 ;
    volatile uint8_t *portx_p2 ;
};


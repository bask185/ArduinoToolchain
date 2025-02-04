#ifndef LEDBLINK_H
#define LEDBLINK_H

#include <Arduino.h>

const int  OFF          = 0 ;
const int  ON           = 1 ;
const int  BLEEPING     = 2 ;
const int  SLOW         = 3 ;
const int  FAST         = 4 ;
const int  REALLY_FAST  = 5 ;


class LedBlink
{
public:
    LedBlink(uint8_t pin);

    void begin();

    void bleep( uint8_t ); // keeps bleeping x amounf of time every cycle
    void turn(  uint8_t );      // on or off
    void blink( uint8_t );
    void setEventBleeps(uint8_t eventBleeps); // also bleeps but just one time, also resets cycle

    void update();

private:

    uint8_t  pin;
    uint8_t  mode;
    uint8_t  bleepCount;
    uint8_t  cycleCounter;
    bool     ledState;

    uint32_t lastTime ;
    bool     eventActive;
    uint8_t  eventBleeps;
    const uint32_t interval = 150 ;
    const uint8_t nCycles   = 20 ;

};

#endif

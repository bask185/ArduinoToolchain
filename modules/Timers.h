#ifndef TIMERS_H
#define TIMERS_H

#include <Arduino.h>

class BaseTimer 
{
protected:
    uint32_t    presetTime ;
    uint32_t    startTime ;
    bool        startTrigger ;
    bool        endTrigger ;

public:
    bool Q ;
    BaseTimer() ;
    virtual uint8_t update(uint8_t IN) = 0 ;
    void setTime( uint32_t presetTime ) ;
};


class TIMER_ON : public BaseTimer 
{
public:
    TIMER_ON() {}
    uint8_t update(uint8_t IN) override;
};


class TIMER_OFF : public BaseTimer 
{
public:
    TIMER_OFF() {}
    uint8_t update(uint8_t IN) override;
};


class TIMER_BLEEP : public BaseTimer 
{
public:
    BlinkTimer() {}
    uint8_t update(uint8_t IN) override;
};


class TIMER_PULSE : public BaseTimer 
{
public:
    pulseTimer() {}
    uint8_t update(uint8_t IN) override;
};

#endif // TIMERS_H

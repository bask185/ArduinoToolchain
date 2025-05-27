#ifndef TIMERS_H
#define TIMERS_H

#include <Arduino.h>

class BaseTimer 
{
public:
    uint32_t    PT ;
    uint32_t    ET ;
    bool        Q ;

    BaseTimer() ;
    virtual uint8_t update(uint8_t IN) = 0 ;
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
    TIMER_BLEEP() {}
    uint8_t update(uint8_t IN) override;
};


class TIMER_PULSE : public BaseTimer 
{
public:
    TIMER_PULSE() {}
    uint8_t update(uint8_t IN) override;
};

#endif // TIMERS_H

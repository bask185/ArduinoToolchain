#ifndef TIMERS_H
#define TIMERS_H

#include <Arduino.h>

class BaseTimer 
{
protected:
    uint32_t presetTime;
    uint32_t startTime;
    bool startTrigger;
    bool endTrigger;

public:
    bool Q;
    BaseTimer(uint32_t presetTime) : presetTime(presetTime), startTime(0), startTrigger(false), endTrigger(true), Q(false) {}
    virtual uint8_t update(uint8_t IN) = 0;
    virtual ~BaseTimer() {}
};

class TON_timer : public BaseTimer 
{
public:
    TON_timer(uint32_t presetTime) : BaseTimer(presetTime) {}
    uint8_t update(uint8_t IN) override;
};

class TOFF_timer : public BaseTimer 
{
public:
    TOFF_timer(uint32_t presetTime) : BaseTimer(presetTime) {}
    uint8_t update(uint8_t IN) override;
};

class BlinkTimer : public BaseTimer 
{
public:
    BlinkTimer(uint32_t presetTime) : BaseTimer(presetTime) {}
    uint8_t update(uint8_t IN) override;
};

#endif // TIMERS_H

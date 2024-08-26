#ifndef TIMERS_H
#define TIMERS_H

#include <Arduino.h>

// Klasse voor de On-Delay Timer (TON)
class TON_timer
{
public:
    TON_timer(uint32_t presetTime);
    uint8_t update(uint8_t IN);

    uint8_t Q : 1;  // Output

private:
    uint32_t startTime ;
    uint32_t presetTime ;
    uint8_t active : 1 ;
    uint8_t triggered : 1 ;  // Geeft aan of de timer al getriggerd is
};

// Klasse voor de Off-Delay Timer (TOFF)
class TOFF_timer
{
public:
    TOFF_timer(uint32_t presetTime);
    uint8_t update(uint8_t IN);

    uint8_t Q : 1;  // Output

private:
    uint32_t startTime ;
    uint32_t presetTime ;
    uint8_t active : 1 ;
    uint8_t triggered : 1 ;  // Geeft aan of de timer al getriggerd is
};

#endif // TIMERS_H
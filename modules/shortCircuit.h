#include <Arduino.h>

extern void shortCircuitInit(uint8_t, uint8_t, uint8*, uint32_t, uint32_t, uint8_t);
// max current is the adc value corresponding with the provided current and resistande
// the calculation:  max adc value = ( 1024 * I * R ) / 5
// we do not work with decimal values, there for we work with milli Amperes and milli Ohms

extern uint8_t shortCircuit();
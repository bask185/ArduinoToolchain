#include <Arduino.h>

class Weistra {
public:
    Weistra(unsigned char);
    void begin();
    void update();
    void setSpeed(unsigned char);

private:
    unsigned int  intervalTime;
    unsigned char dutyCycle;
    unsigned char trackPin;

    volatile uint8_t *portx_p;
};


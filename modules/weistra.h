#include <Arduino.h>

class Weista {
public:
    Weistra(byte);
    void begin();
    void update();
    void setSpeed(byte);
}

private:
    uint16_t intervalTime;
    byte dutyCycle;
    byte trackPin;
    byte *port;
};


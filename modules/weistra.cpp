#include "weistra.h"

#define Fmax 100
#define Fmin 50

Weistra::Weistra(unsigned char _pin) { // constructor
    trackPin = _pin;
} 

void Weistra::begin() {
    pinMode(trackPin, OUTPUT);

    byte port   = digitalPinToPort( trackPin );
    trackPin    = digitalPinToBitMask( trackPin );
    portx_p     = portOutputRegister( port );

}

void Weistra::update() {
    static byte counter = 0;
    static uint32_t prevTime = 0;

    if( portx_p != 0 ) {
        uint32_t currentTime = micros()/* & 0x0000FFFF*/; // we only use the last 2 bytes of micros()

        if( currentTime - prevTime >= intervalTime ) { // interval
            prevTime = currentTime;

            
            if( counter == 0 && dutyCycle > 0) {  // if counter reaches 100, reset it to 0 and enable the track power pin
                *portx_p |=  trackPin;
            }
            if( counter >= dutyCycle && dutyCycle < 100) {
                *portx_p &= ~trackPin;
            }
            counter++;
            if(counter > 100) counter = 0;
        }
    }
}


void Weistra::setSpeed(byte speed) {
    byte frequency;

    dutyCycle = constrain(speed, 0, 100); // speed limit = 0 - 100

    if( dutyCycle <= 10 ) { frequency = Fmin; }
    else                  { frequency = map(dutyCycle, 10, 100, Fmin, Fmax); }

    intervalTime = 10000 / frequency; // > between 100us and 500us
 }


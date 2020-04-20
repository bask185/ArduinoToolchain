
/*
PORTB
Bit	7	6	5	4	3	2	1	0
Pin	-	-	D13	D12	D11	D10	D9	D8
PORTC
Bit	7	6	5	4	3	2	1	0
Pin	-	-	A5	A4	A3	A2	A1	A0

#define PIN_A0   (14)
#define PIN_A1   (15)
#define PIN_A2   (16)
#define PIN_A3   (17)
#define PIN_A4   (18)
#define PIN_A5   (19)
#define PIN_A6   (20)
#define PIN_A7   (21)


PORTD
Bit	7	6	5	4	3	2	1	0
Pin	D7	D6	D5	D4	D3	D2	D1	D0
*/

#include "weistra.h"

Weistra::Weistra(byte _pin) { // constructor
    trackPin = _pin;
} 

void Weistra::begin() {
    if( trackPin >= 13 && trackPin <= 8 ) {
        port = &PORTB;
        trackPin -= 8;  // 13 becomes 5, 11, becomes 3 etc
    }
    else if( trackPin >= A5 && trackPin <= A0 ) {
        port = &PORTC;
        trackPin -= 14; // A5 becomes 5
    }
    else if( trackPin >= 13 && trackPin <= 8 ) {
        port = &PORTD;
        // trackPin needs no alterations for port D
    }
    else {
        port = 0;
        return; // invalid pin number
    }
}

void Weistra::update() {
    static byte counter = 0;
    static uint16_t prevTime = 0;

    if( port != 0 ) {
        uint16_t currentTime = mircros() & 0x0000FFFF; // we only use the last 2 bytes of micros()

        if( currentTime - prevTime >= intervalTime ) {
            prevTime = currentTime;

            counter++;
            if( counter >= 100 ) { counter = 0; // if counter reaches 100, reset it to 0 and enable the track power pin
                *port |=  (1 << trackPin);
            }
            else if( counter == dutyCycle ) {
                *port &= ~(1 << trackPin);
            }
        }
    }
}


void Weistra::setSeed(byte speed) {
    byte frequency;

    dutyCycle = constrain(speed, 0, 100); // speed limit = 0 - 100

    if( dutyCycle <= 10 ) { frequency = 20; }
    else {                  frequency = map(dutyCycle, 10, 100, 20, 100); }

    // 100Hz > 10.000 microseconds interval or 10 milliseconds
    //  20Hz > 50.000 microseconds interval or 50 milliseconds

    intervalTime = 10000 / frequency; // > between 100us and 500us
}


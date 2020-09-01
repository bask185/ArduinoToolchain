#include <Arduino.h>
#include <Servo.h>

class ServoSweep {

public:
    ServoSweep( uint8_t _servoPin, uint8_t _min, uint8_t _max, uint8_t _speed, uint8_t _turnOff  ) ;        // constructor 1
    ServoSweep( uint8_t _servoPin, uint8_t _min, uint8_t _max, uint8_t _speed, uint8_t _turnOff, uint8_t _relayPin ) ;  // constructor 2
    uint8_t sweep( );
    void setState( uint8_t _state );
    // void turnOn() ;
    // void turnOff() ;
    void begin( );

private:
    Servo servo ;
    unsigned long timeToRun ;
    uint8_t pos ;
    uint8_t state ;
    uint8_t prevPos;
    uint8_t servoPin ;
    uint8_t servoSpeed ;
    uint8_t servoMin ;
    uint8_t servoMax  ;
    uint8_t middlePosition ;
    uint8_t relayPresent ;
    uint8_t relayPin ;
    uint8_t turnOff ;
    unsigned long currentTime = millis() ;
} ;

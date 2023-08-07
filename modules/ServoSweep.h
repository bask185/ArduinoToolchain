#include <Arduino.h>
#include <Servo.h>

const int STORE_POSITIONS   = 0b00000001 ;
const int DEFAULT_BIT       = 0b10000000 ;

class ServoSweep {

public:
    ServoSweep( uint8_t _servoPin, uint8_t _min, uint8_t _max, uint8_t _speed, uint8_t _turnOff  ) ;        // constructor 1
    ServoSweep( uint8_t _servoPin, uint8_t _min, uint8_t _max, uint8_t _speed, uint8_t _turnOff, uint8_t _relayPin ) ;  // constructor 2
    uint8_t sweep( );
    void setState( uint8_t _state );
    void begin( );
    void useEEPROM( uint16_t, uint8_t ) ;
    void setMin( uint8_t _min) ;
    void setMax( uint8_t _max) ;

private:
    Servo servo ;
    uint32_t lastTime ;
    uint8_t pos ;
    uint8_t state ;
    uint8_t prevPos;
    uint8_t servoPin ;
    uint8_t servoSpeed ;
    uint8_t servoMin ;
    uint8_t servoMax  ;
    uint8_t middlePosition ;
    uint8_t relayPin ;
    uint8_t turnOff ;
    uint8_t eeFlags ;
    uint16_t eeAddress = 0xFFFF ;
} ;

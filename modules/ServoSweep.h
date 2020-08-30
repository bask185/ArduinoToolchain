#include <Arduino.h>
//#include <Servo.h>

class ServoSweep {

public:
    ServoSweep( byte _min, byte _max, byte _speed  ) ;        // constructor 1
    ServoSweep( byte _min, byte _max, byte _speed, byte _relayPin ) ;  // constructor 2
    uint8_t sweep( );
    void setState( uint8_t _state );

private:
   // Servo servo ;
    unsigned long timeToRun ;
    byte pos ;
    byte state ;
    byte prevPos;
    byte servoPin ;
    byte servoSpeed ;
    byte servoMin ;
    byte servoMax  ;
    byte middlePosition ;
    byte relayPresent ;
    byte relayPin ;
    unsigned long currentTime = millis() ;

} ;

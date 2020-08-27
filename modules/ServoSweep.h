#include <Arduino.h>
#include <Servo.h>

class ServoSweep {

public:
    ServoSweep( byte _servoPin, byte _inputPin, byte _min, byte _max, byte _speed  ) ;        // constructor 1
    //ServoSweep( byte _servoPin, byte _inputPin, byte _min, byte _max, byte _speed, _relayPin ) ;  // constructor 2
    void sweep();

private:
    Servo servo ;
    unsigned long timeToRun ;
    byte inputPin ;
    byte state ;
    byte pos ;
    byte prevPos;
    byte servoPin ;
    byte servoSpeed ;
    byte servoMin ;
    byte servoMax  ;
    byte middlePosition ;
    byte relayPresent ;
    byte relayPin ;

} ;


#include <Arduino.h>
#include <Servo.h>

class ServoSweep {

public:
    ServoSweep( byte _servoPin, byte _min, byte _max, byte _speed  ) ;        // constructor 1
    ServoSweep( byte _servoPin, byte _min, byte _max, byte _speed, byte _relayPin ) ;  // constructor 2
    void sweep( );
    void setState( bool _state );

private:
    Servo servo ;
    unsigned long timeToRun ;
    byte pos ;
    bool state ;
    byte prevPos;
    byte servoPin ;
    byte servoSpeed ;
    byte servoMin ;
    byte servoMax  ;
    byte middlePosition ;
    byte relayPresent ;
    byte relayPin ;

} ;


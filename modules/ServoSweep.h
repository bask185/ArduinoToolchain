#include <Arduino.h>
#include <Servo.h>

class {
public:
    ServoSweep();
    sweep();

private:
    enum servoStates {
        moveLeft,
        moveRight,
    } ;

    Servo servo ;
    unsigned long timeToRun ;
    byte servoPin ;
    byte inputPin ;
    byte state ;
    byte pos ;
    byte prevPos;
    byte servoPin ;
    byte servoSpeed ;
    byte servoMin ;
    byte servoMax  ;
} ;
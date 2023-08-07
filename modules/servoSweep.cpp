#include "ServoSweep.h"

#define movingLeft 0
#define movingRight 1


ServoSweep::ServoSweep( uint8_t _servoPin, uint8_t _min, uint8_t _max, uint8_t _turnOff,  uint8_t _speed ) {                   // constructor 1
    
    servoPin = _servoPin ;
    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;
   
    middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;               // start with middle position

    relayPin = 0xFF ; // no relay

    if( _turnOff ) turnOff = 1 ;
    else           turnOff = 0 ;
}

ServoSweep::ServoSweep( uint8_t _servoPin, uint8_t _min, uint8_t _max, uint8_t _speed, uint8_t _turnOff, uint8_t _relayPin ) {      // constructor 2
    
    servoPin = _servoPin ;
    servoSpeed = _speed ;
    servoMin = _min ;
    servoMax = _max ;

    middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;

    relayPin = _relayPin ;

    if( _turnOff ) turnOff = 1 ;
    else           turnOff = 0 ;
}

void ServoSweep::begin()
{
    if( eeAddress != 0xFFFF ) // If EEPROM present
    {
        if( eeFlags & DEFAULT_BIT ) // if this bit is set, we must initialize the EEPROM
        {
            EEPROM.update( eeAddress+0, servoMin ) ;
            EEPROM.update( eeAddress+1, servoMax ) ;
            EEPROM.update( eeAddress+2,        0 ) ; // last state
        }

        servoMin = EEPROM.read( eeAddress+0 ) ;
        servoMax = EEPROM.read( eeAddress+1 ) ;
        middlePosition = ( (long)servoMax - (long)servoMin ) / (long)2 + (long)servoMin ;

        if( eeFlags & STORE_POSITIONS )
        {
            state = EEPROM.read(eeAddress+2) ;
        }
    }

    if( relayPin != 0xFF ) pinMode( relayPin, OUTPUT ) ;
}

void ServoSweep::setState( uint8_t _state )
{
    state = _state ;
    if( turnOff ) servo.attach( servoPin ) ;

    if( eeFlags & STORE_POSITIONS )
    {
        EEPROM.update( eeAddress+2, state ) ;
    }
}

void ServoSweep::setMin( uint8_t _min)
{
    servoMin = _min ;
    EEPROM.update( eeAddress+0, servoMin ) ;
}

void ServoSweep::setMax( uint8_t _max)
{
    servoMax = _max ;
    EEPROM.update( eeAddress+1, servoMax ) ;
}

uint8_t ServoSweep::sweep ( )
{
    if( millis() - lastTime > servoSpeed )
    {       lastTime = millis() ;
     
        uint8_t setPoint ;

        if( state ) setPoint = servoMax ; // get set point
        else        setPoint = servoMin ;

        if( pos < setPoint ) pos ++ ;   // follow positon to setpoint
        if( pos > setPoint ) pos -- ;

        if( prevPos != pos ) {          // if position has changed.
            prevPos  = pos ;

            if( servoPin != 255 ) servo.write( pos ) ;

            if( pos != setPoint 
            &&  servo.attached() == false ) // attach motor if needed
            {
                servo.attach( servoPin ) ;
            }

            if( pos == setPoint 
            &&  turnOff == 1 )
            {
                servo.detach( ) ; // detach motor if needed
            }

            if( relayPin != 0xFF ) // SK: todo, add something that relay does not change direction
            {                      //           when servo min is smaller than max
                if( pos < middlePosition ) digitalWrite( relayPin,  LOW ) ;
                else                       digitalWrite( relayPin, HIGH ) ;
            }
            
            return pos ;
        }
        else
        {
            return 0 ;
        }
    }
}

void ServoSweep::useEEPROM( uint16_t _eeAddress, uint8_t _eeFlags )
{
    eeAddress = _eeAddress ;
    eeFlags = _eeFlags ;
}
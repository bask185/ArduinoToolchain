#include "Stepper.h"
#include <EEPROM.h>

Stepper::Stepper(void (*pulseFunction)(uint8_t dir))
{
    homeSensorPin = 0x7F ; // defaults, should be overwritten in setup
    acceleration =     1 ;
    maxSpeed     =    10 ;
    pulsePerRev  =   200 ;
    eepromOffset =     0 ;

    pulse = pulseFunction ;
}

void Stepper::setMode( uint8_t _mode )
{
    if( _mode == ROTATING_MODE ) rotatingMode = 1 ;
    if( _mode ==   LINEAR_MODE )   linearMode = 1 ;
}

uint8_t Stepper::limitCheck() {
    int32_t brakeDistance = speed * acceleration ;
    int32_t brakePos;

    if (currentDir == CW) {
        brakePos = targetPosition - brakeDistance;
        if (brakePos < 0) {
            brakePos += limitPosition;
        }
    } else {
        brakePos = targetPosition + brakeDistance;
        if (brakePos >= limitPosition) {
            brakePos -= limitPosition;
        }
    }

    if (currentDir == CW ) {
        if ((currentPosition >= brakePos && currentPosition <= targetPosition) || 
            (brakePos > targetPosition && (currentPosition >= brakePos || currentPosition <= targetPosition))) {
            mode = DECELERATING;
            return true;
        }
    } 
    else {
        if ((currentPosition <= brakePos && currentPosition >= targetPosition) || 
            (brakePos < targetPosition && (currentPosition <= brakePos || currentPosition >= targetPosition))) {
            mode = DECELERATING;
            return true;
        }
    }

    return false;
}

void Stepper::setEepromOffset(uint16_t _offset)
{
    eepromOffset = _offset ;
}

void Stepper::home()
{
    if( homeSensorPin == 0xFF ) return ;
    homingState = 1 ;
}

void Stepper::homeCycle()
{
    switch( homingState )
    {

    case 1: // spoof current position and start cruising towards home position.
        currentPosition = 0x7FFFFFFF ;
        cruise( CCW ) ;
        homingState ++ ;
        break ;

    case 2: // wait until sensor is reached 
        if( digitalRead( homeSensorPin ) == (LOW ^sensorInverted)) 
        {
            stop() ;
            homingState ++ ;
        }
        break ;

    case 3: // wait until speed = 0 ;
        if( speed == 0 ) { homingState ++ ;}
        break ;

    case 4:
        cruise( CW ) ;      // start cruising back again at lower speed
        // TODO, fix speed (perhaps do in update...)
        if( digitalRead( homeSensorPin ) == (HIGH^sensorInverted) ) homingState += 1 ;
        else                                                        homingState += 2 ;
        break ;

    case 5: // if sensor has been passed during stopping we first need to re-enter the sensor again
        if( digitalRead( homeSensorPin ) == (LOW^sensorInverted) ) { homingState ++ ; }
        break ;

    case 6: // wait until we have moved away from the home sensor
        if( digitalRead( homeSensorPin ) == (HIGH^sensorInverted) )
        {
            stop() ;
            homingState ++ ;
        }
        break ;
    
    case 7: // wait until we have stopped moving again.
         if( speed == 0 )
         {
            homingState ++ ;
            cruise( CCW ) ;
         }

         break ;

    case 8: // final approach at low speed to sensor
        if( digitalRead( homeSensorPin ) == (LOW^sensorInverted ))
        {
            stop() ;
            homingState ++ ;
        }
        break ;

    case 9: // wait untill we have stopped again
        if( speed == 0 )
        {
            currentPosition = 0 ;
            homingState = 0 ;
        }
        break ;
    }
}

uint8_t Stepper::update()
{
    uint64_t now = micros();

    homeCycle() ;

    if( mode == IN_POSITION
    && currentPosition != targetPosition
    && stopped == 0 )
    {
        mode = ACCELERATING;
        stepDelay = 0;
        lastMicros = now;
        now++;
    }

    if (now - lastMicros >= stepDelay && mode != IN_POSITION )
    {
        lastMicros = now;

        if (mode == ACCELERATING)
        {
            if (speed < maxSpeed)
            {
                speed += acceleration;
                int16_t speedTemp = maxSpeed ;
                if( homingState >= 3 ) speedTemp = maxSpeed / 3 ;
                if (speed > speedTemp) speed = speedTemp;
            }
            if( limitCheck() )     { mode = DECELERATING; }
            if (speed == maxSpeed) { mode = CRUISING; }
        }

        if (mode == DECELERATING)
        {
            if (speed > 0)
            {
                speed -= acceleration;
                if (speed < 0) speed = 0;
            }

            if (speed == 0 ) { mode = IN_POSITION; }
        }

        if (mode == CRUISING)
        {
            if( (linearMode || rotatingMode) && limitCheck() ) { mode = DECELERATING; }
        }
        
        if( velocityMode )
        {
            if( speed < targetVelocity ) speed += acceleration;
            if( speed > targetVelocity ) speed -= acceleration;
            currentDir = (speed > 0) ? CW : CCW;

            if( stopped == 1 && speed == 0 ) velocityMode = 0 ;
        }

        if(abs(speed) > 0) stepDelay = (uint32_t)((uint32_t)1000000 * (uint32_t)60) / ((uint32_t)abs(speed) * (uint32_t)pulsePerRev);
        else               stepDelay = 1000000;

        if(( currentPosition != targetPosition ) 
        ||   (velocityMode && abs(speed) > 0))
        {  
            if (currentDir == CW) { currentPosition ++ ; } 
            else                  { currentPosition -- ; }

            if( rotatingMode )
            {
                if(      currentPosition == limitPosition ) currentPosition =             0 ;
                else if( currentPosition ==            -1 ) currentPosition = limitPosition ;
            }

            pulse(currentDir);
        }
        else
        {
            if (awoff) { pulse( AWOFF ) ; }
            else       { pulse(  HOLD ) ; } 
        }
    }
    return mode;
}


void Stepper::setMaxSpeed(int16_t _speed)
{
    maxSpeed = _speed;
    speed_set = 1 ;
}

void Stepper::setVelocity( int16_t _speed ) 
{
    if( linearMode ) return ;

    targetVelocity = _speed ;
    velocityMode = 1 ;
    stopped = 0 ;
    mode = 8 ;  // to prevent the update function from working against velocityMode
}               // we put the mode in an undefined state. It is a bit dirty, but is an easy fix non the less

void Stepper::setPulsesPerRevolution(uint16_t _pulsePerRev)
{
    pulsePerRev = _pulsePerRev ;
    pulses_set = 1 ;
}

void Stepper::setHomeSensor(uint8_t _pin, uint8_t _inv)
{
    homeSensorPin = _pin;
    if( _inv )  sensorInverted = 1 ;
    else        sensorInverted = 0 ;
}

void Stepper::setAWOFF()
{
    awoff = 1;
}

void Stepper::setLimit( int32_t _limit )
{
    int eeAddress = eepromOffset + 8;
    limitPosition = _limit;
    EEPROM.put(eeAddress, limitPosition);
}

// void Stepper::setCirumReference( int32_t _circumreference )
// {
//     limitPosition = _circumreference ;
// }

void Stepper::setAcceleration(uint16_t _acceleration)
{
    acceleration = _acceleration;
    acceleration_set = 1 ;
}

void Stepper::moveAbs( int32_t _setpoint)
{
    if( rotatingMode ) return ;  // not used in rotating mode

    stopped = 0 ;

    targetPosition = _setpoint ;

    uint8_t newDir = (targetPosition < currentPosition) ? CCW : CW;
    if( currentDir != newDir ) mode = DECELERATING ;
    currentDir = newDir ;
    
    targetPosition = constrain( targetPosition, 0, limitPosition ) ;
}

// move this fixed relative position
void Stepper::moveRel( int32_t _relativePos)
{
    stopped = 0 ;
    targetPosition = currentPosition + _relativePos;

    uint8_t newDir = (_relativePos > 0) ? CW : CCW ;
    if( currentDir != newDir ) mode = DECELERATING ;
    currentDir = newDir ;

    if( rotatingMode )
    {
        if( currentDir ==  CW && targetPosition > limitPosition ) targetPosition -= limitPosition ; // handle overflow for rotating mode
        if( currentDir == CCW && targetPosition <             0 ) targetPosition += limitPosition ;  
    }

    if( linearMode ) // in linear mode we constrain the limit position to minimum and maximum values
    {
        targetPosition = constrain( targetPosition, 0, limitPosition ) ;
    }
}

void Stepper::cruise(uint8_t _dir)
{   
    if( linearMode )
    {
        if( _dir ==  CW ) moveAbs( 0x7FFF ) ;
        if( _dir == CCW ) moveAbs(      0 ) ;
    }
    if( rotatingMode )
    {
        velocityMode = 1 ;
        if( _dir ==  CW ) setVelocity(  maxSpeed ) ;
        if( _dir == CCW ) setVelocity( -maxSpeed ) ;
    }
}

void Stepper::stop()
{
    targetVelocity = 0 ;
    mode = DECELERATING;
    stopped = 1 ;
}

int32_t  Stepper::getLimit()               { return limitPosition;  }
uint8_t  Stepper::getStatus()              { return mode;           }
int16_t  Stepper::getSpeed()               { return speed;          } 
int32_t  Stepper::getPosition()            { return currentPosition;} 
int16_t  Stepper::getMaxSpeed()            { return maxSpeed;       } 
uint16_t Stepper::getAcceleration()        { return acceleration;   }
uint8_t  Stepper::getHomeSensor()          { return homeSensorPin;  }
uint8_t  Stepper::getAWOFF()               { return awoff;          }
uint16_t Stepper::getPulsesPerRevolution() { return pulsePerRev;    }
uint16_t Stepper::getEepromOffset()        { return eepromOffset;   }

void Stepper::Estop()
{
    eStopFlag = 1;
}

void Stepper::startTeachin()
{
    teachinFlag = 1;
    nStoredPositions = 0;
    limitPosition = 0x7FFFFFFF ;
}

void Stepper::stopTeachin()
{
    int eeAddress = eepromOffset + 0;
    EEPROM.put(eeAddress, nStoredPositions);

    setLimit( currentPosition ) ;

    teachinFlag = 0;
}

void Stepper::storePosition()
{
    if( teachinFlag == 0 ) return ;

    if (nStoredPositions < MAX_POSITIONS)
    {
        int eeAddress = eepromOffset + 20 + (nStoredPositions * sizeof(currentPosition));
        EEPROM.put(eeAddress, currentPosition);
        nStoredPositions++;        
    }
    else if( errorCallback )
    {
        errorCallback("max positions reached");
    }
}

uint8_t Stepper::shift(uint8_t dir)
{
    if( teachinFlag == 0 )
    {
        currentDir = dir ;

        if (dir == CW)
        {   if( rotatingMode ) if(++currentIndex  ==  nStoredPositions     ) currentIndex = 0 ; 
            if( linearMode   ) if( currentIndex    < (nStoredPositions - 1)) currentIndex  ++ ;
        }
        else
        {
            if( rotatingMode ) if(--currentIndex  == -1) currentIndex = nStoredPositions - 1;
            if( linearMode   ) if( currentIndex    >  0) currentIndex  -- ;
        }

        int32_t newPosition ;
        int eeAddress = eepromOffset + 20 + (currentIndex * sizeof(newPosition));
        EEPROM.get(eeAddress, newPosition);
        
        eeAddress = eepromOffset + 4;
        EEPROM.put(eeAddress, currentIndex);

        moveAbs( newPosition ) ;
    }
    else
    {
        if (dir == CW) { moveRel(  maxSpeed * acceleration ) ; } 
        else {           moveRel( -maxSpeed * acceleration ) ; } 
    }
}

void Stepper::setIndex( uint8_t index ) // UNTESTED
{
    index = max( index, nStoredPositions - 1 ) ;
    currentIndex = index ;
    int eeAddress = eepromOffset + 20 + (index * sizeof(currentPosition));

    EEPROM.get(eeAddress, targetPosition);

    if( linearMode )
    {
        if( targetPosition < currentPosition ) currentDir = CCW ;
        else                                   currentDir =  CW ;
    }
    
    if( rotatingMode )
    {
        int32_t normCurrentPosition = (currentPosition % limitPosition + limitPosition) % limitPosition;
        int32_t  normTargetPosition = (targetPosition  % limitPosition + limitPosition) % limitPosition;

        int32_t cwDistance =  (normTargetPosition >= normCurrentPosition) 
                            ? (normTargetPosition  - normCurrentPosition) 
                            : (limitPosition - normCurrentPosition + normTargetPosition);

        int32_t ccwDistance = (normCurrentPosition >= normTargetPosition) 
                            ? (normCurrentPosition  - normTargetPosition) 
                            : (limitPosition - normTargetPosition + normCurrentPosition);

        if (cwDistance <= ccwDistance){
            currentDir = CW;
        } else {
            currentDir = CCW;
        }
    }

    stopped = 0 ;
}

uint8_t Stepper::getIndex() { return currentIndex ; }

void Stepper::whipeEeprom()
{
    int eeAddress = eepromOffset + 0;
    nStoredPositions = 0xFF ;
    currentIndex     = 0xFF ;
    limitPosition    = 0xFFFFFFFF ;

    EEPROM.put(eeAddress, nStoredPositions);

    eeAddress = eepromOffset + 4;
    EEPROM.put(eeAddress, currentIndex);

    eeAddress = eepromOffset + 20 + (currentIndex * sizeof(currentPosition));
    EEPROM.put(eeAddress, currentPosition); 

    eeAddress = eepromOffset + 8;
    EEPROM.put(eeAddress, limitPosition);

    begin() ; // invoke begin to load defaults.
}

void Stepper::begin()
{
    int eeAddress = eepromOffset + 0;
    EEPROM.get(eeAddress, nStoredPositions);

    eeAddress = eepromOffset + 4;
    EEPROM.get(eeAddress, currentIndex);

    eeAddress = eepromOffset + 20 + (currentIndex * sizeof(currentPosition));
    EEPROM.get(eeAddress, currentPosition); 

    eeAddress = eepromOffset + 8;
    EEPROM.get(eeAddress, limitPosition);

    if( nStoredPositions == 0xFF 
    ||    currentIndex   == 0xFF
    ||   limitPosition   == 0xFFFFFFFF )
    {
        eeAddress = eepromOffset + 0;
        nStoredPositions = 0 ;
        EEPROM.put(eeAddress, nStoredPositions);

        eeAddress = eepromOffset + 4;
        currentIndex = 0 ;
        EEPROM.put(eeAddress, currentIndex);

        eeAddress = eepromOffset + 8;
        limitPosition = 0x7FFFFFFF ;
        EEPROM.put(eeAddress, limitPosition);

        if(errorCallback ) errorCallback("WARNING EEPROM DEFAULTS INITIALIZED") ;

        currentPosition = 0x7FFFFFFF ;
        for( int i = 0 ; i < MAX_POSITIONS ; i ++ )
        {
            eeAddress = eepromOffset + 20 + (currentIndex * sizeof(currentPosition)); // whipe all stored positions
            EEPROM.put(eeAddress, currentPosition) ;
        }

        currentPosition = 0;
        targetPosition = 0;
    }
    else
    {
        eeAddress = eepromOffset + 20 + (currentIndex * sizeof(currentPosition));
        EEPROM.get(eeAddress, currentPosition) ;
    }



    mode = IN_POSITION;



    targetPosition = currentPosition ;
    stopped = 1 ;

    if(( (!rotatingMode) && (!linearMode) )     
    && errorCallback )                         errorCallback("ERROR no valid mode set") ;
    if( !pulses_set         && errorCallback ) errorCallback("ERROR Pulses per revolution not set") ;
    if( !acceleration_set   && errorCallback ) errorCallback("ERROR acceleration not set") ;
    if( !speed_set          && errorCallback ) errorCallback("ERROR max speed not set") ;
    if( homeSensorPin==0x7F && errorCallback ) errorCallback("WARNING no home sensor pin defined") ;
}

void Stepper::setErrorCallback(void (*callback)(const char* errorMessage))
{
    errorCallback = callback;
}

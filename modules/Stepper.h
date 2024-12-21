#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>

/// @brief Stepper modes and states for various operations
// every object uses around 45 bytes of RAM
enum
{
    CW,               ///< Clockwise rotation
    CCW,              ///< Counterclockwise rotation
    AWOFF,            ///< Anti-Windup Off mode
    HOLD,             ///< Hold position mode

    ACCELERATING,     ///< Acceleration mode
    DECELERATING,     ///< Deceleration mode
    CRUISING,         ///< Cruising speed mode
    IN_POSITION,      ///< Stepper is in position

    ROTATING_MODE,
    LINEAR_MODE,

};

const int MAX_POSITIONS = 64; ///< Maximum number of storable positions

/// @brief Class for controlling a stepper motor with various movement functions and error handling
class Stepper
{
public:
    /**
     * @brief Constructor for Stepper class
     * @param pulse Callback function for pulsing stepper direction
     */
    Stepper(void (*pulseFunction)(uint8_t dir), uint8_t _mode);

    /**
     * @brief Initializes the stepper motor settings
     */
    void        begin();

    /**
     * @brief Sets the maximum speed in RPM
     * @param _speed Maximum speed value
     */
    void        setMaxSpeed(int16_t _speed); 

    /**
     * @brief Sets the limit position
     * @param _limit Position limit value
     */
    void        setLimit( int32_t _limit) ;

    /**
     * @brief Sets the circumference reference for calculating movement
     * @param _circumReference Circumference reference value
     */
    // void        setCirumReference( int32_t _circumReference ) ;

    /**
     * @brief Sets the acceleration value
     * @param _acceleration Acceleration value
     */
    void        setAcceleration(uint16_t _acceleration);

    /**
     * @brief Sets the home sensor pin
     * @param _pin Home sensor pin number
     */
    void        setHomeSensor(uint8_t _pin, uint8_t _inv);

    /**
     * @brief Enables Anti-Windup Off mode
     */
    void        setAWOFF();

    /**
     * @brief Sets the pulses per revolution
     * @param _pulsePerRev Number of pulses per revolution
     */
    void        setPulsesPerRevolution(uint16_t _pulsePerRev);

    /**
     * @brief Sets the velocity of the stepper
     * @param _velocity Velocity value
     */
    void        setVelocity(int16_t _velocity);

    /**
     * @brief Sets Eeprom address offset for when one more than 1 motor is used, every object uses 272 bytes
     * @param _offset address offset value
     */
    void        setEepromOffset(uint16_t _offset);

    // Getter functions
    int16_t     getMaxSpeed();
    int32_t     getLimit();
    uint16_t    getAcceleration();
    uint8_t     getHomeSensor();
    uint8_t     getAWOFF();
    uint16_t    getPulsesPerRevolution();
    int16_t     getSpeed();
    int32_t     getPosition();
    uint8_t     getStatus();
    uint16_t    getEepromOffset();

    /**
     * @brief Updates the stepper motor status
     * @return Status byte indicating the current state
     */
    uint8_t     update();

    /**
     * @brief Moves the stepper to the home position
     * @param _homingSpeed Speed used for homing
     * @return Status byte indicating the success of homing operation
     */
    uint8_t     move2home(uint16_t _homingSpeed);

    /**
     * @brief Moves the stepper motor to an absolute position
     * @param _setpoint The target absolute position
     */
    void        moveAbs(int32_t _setpoint) ;

    /**
     * @brief Moves the stepper motor to a relative position
     * @param _targetPos The target position relative to the current position
     */
    void        moveRel(int32_t _targetPos) ;

    /**
     * @brief Sets the stepper motor to cruise at a specified direction
     * @param _dir Direction of cruising (1 for CW, 0 for CCW)
     */
    void        cruise(uint8_t _dir) ;

    /**
     * @brief Stops the stepper motor smoothly
     */
    void        stop() ;

    /**
     * @brief Executes an emergency stop for the stepper motor
     */
    void        Estop() ;

    /**
     * @brief Initiates the homing sequence to find the home position
     */
    void        home() ;


    /**
     * @brief Starts teaching mode for recording positions
     */
    void        startTeachin();

    /**
     * @brief Stops teaching mode
     */
    void        stopTeachin();

    /**
     * @brief Stores the current position
     */
    void        storePosition();

    /**
     * @brief Shifts to the next or previous position
     * @param dir Direction to shift (1 for forward, 0 for backward)
     * @return Status byte indicating the success of the operation
     */
    uint8_t     shift(uint8_t dir);

    /**
     * @brief Dumps the stored positions to serial output
     */
    void        dumpTeach() ;

    /**
     * @brief Sets the current index for position storage
     * @param index Index value to set
     */
    void        setIndex( uint8_t index );

    /**
     * @brief Gets the current index for position storage
     * @return Current index value
     */
    uint8_t     getIndex() ;

    /**
     * @brief Whipes EEPROM
     */
    void        whipeEeprom() ;

    /**
     * @brief Sets a callback for handling errors like position lost or limit exceeded
     * @param errorCallback Pointer to callback function
     */
    void        setErrorCallback(void (*errorCallback)(const char* errorMessage));

private:
    /**
     * @brief Checks if the current position is within the set limits
     * @return Status byte indicating if position is within limits
     */
    inline uint8_t limitCheck() ;
    inline void    homeCycle() ;

    int32_t     currentPosition ;
    int32_t     targetPosition ;
    uint16_t    acceleration ;
    int32_t     targetVelocity ;
    int16_t     speed ;
    uint16_t    eepromOffset ;
    int16_t     pulsePerRev ;
    int16_t     maxSpeed ;
    uint32_t    lastMicros ;
    uint32_t    stepDelay ;
    uint8_t     mode = IN_POSITION ;
    uint8_t     currentDir      : 1 ;
    uint8_t     linearMode      : 1 ;
    uint8_t     rotatingMode    : 1 ;
    uint8_t     velocityMode    : 1 ;
    uint8_t     teachinFlag     : 1 ;
    uint8_t     stopped         : 1 ;
    uint8_t     awoff           : 1 ;
    uint8_t     eStopFlag       : 1 ;

    uint8_t     pulses_set      : 1 ;
    uint8_t     acceleration_set: 1 ;
    uint8_t     speed_set       : 1 ;

    uint8_t     homingState = 0 ;
    uint8_t     homeSensorPin   : 7 ;
    uint8_t     sensorInverted  : 1 ;

    uint8_t     nStoredPositions;
    uint8_t     currentIndex;
    int32_t     limitPosition;

    void (*pulse)(uint8_t dir) = nullptr;
    void (*errorCallback)(const char* errorMessage) = nullptr;
};

#endif
#include <Arduino.h>
#include "macros.h"
#include "i2cEeprom.h"

enum eventModes
{
    idle,
    playing,
    recording,
    finishing,
} ;

enum defaultEvents
{
    FEEDBACK = 250,
    START,
    STOP,
} ;

enum eepromTypes
{
    INTERNAL_EEPROM,
    I2C_EEPROM, 
} ;

typedef struct 				// 8 bytes per event
{
	uint8 	data1 ;
	uint16 	data2 ;
	uint8	data3 ;
	uint32  time2nextEvent ;
} Event ;


class EventHandler
{
public:
    EventHandler( uint32, uint32        ) ;
    EventHandler( uint32, uint32, uint8 ) ; // for I2C_EEPROM

    void    startRecording() ;      // need begin function? to init I2c bus??
    void    stopRecording() ;
    void    startPlaying() ;
    void    stopPlaying() ;
    void    resetProgram() ;
    void    sendFeedbackEvent( uint16 ) ;
    void    update() ;
    void    begin() ;
    void    storeEvent( uint8, uint16, uint8 ) ;
    uint8   getState() ;

private:
    Event       event ;
    Event       getEvent() ;
    I2cEeprom   i2cEeprom ;

    uint32  beginAddress ;
    uint8   i2cAddress ;
    uint32  eeAddress ;
    uint32  maxAddress ;
    uint32  prevTime ;
    uint16  newSensor ;
    uint8   recordingDevice ;
    uint8   eepromType ;
};



// callback function
extern void notifyEvent( uint8, uint16, uint8 ) __attribute__ (( weak )) ;
extern void displayFreeMemory(  uint16 )        __attribute__ (( weak )) ;
extern void displayStoreMemory( uint16 )        __attribute__ (( weak )) ;
extern void displayGetMemory(   uint16 )        __attribute__ (( weak )) ;
extern void memoryFull()                        __attribute__ (( weak )) ;
extern void notifyI2cErrror(uint8)              __attribute__ (( weak )) ;

#include <EEPROM.h>
#include "event.h"

extern void printNumberAt(uint8,uint8,uint8,uint16) ;
extern void clearDisplay() ;
EventHandler::EventHandler( uint32 _beginAddress, uint32 _maxAddress )
{
    recordingDevice = idle ;
    beginAddress = _beginAddress ;
    eepromType   = INTERNAL_EEPROM ;
    i2cAddress = 0 ;
    maxAddress   = _maxAddress ;
}

EventHandler::EventHandler( uint32 _beginAddress, uint32 _maxAddress, uint8 _i2cAddress )
{
    recordingDevice = idle ;
    beginAddress = _beginAddress ;
    eepromType   = I2C_EEPROM ;
    i2cAddress   = _i2cAddress ;
    maxAddress   = _maxAddress ;
}

void EventHandler::begin()
{
    i2cEeprom.begin( i2cAddress ) ;
}


void EventHandler::startRecording() 
{
    if( recordingDevice == idle )
    {
        eeAddress = beginAddress ;                     // set EEPROM adres to 0
        recordingDevice = recording ;       
        prevTime = millis() ;                           // record starting time
        storeEvent( START, 1, 1 ) ;
    }
}

void EventHandler::stopRecording() 
{
    if( recordingDevice == recording )
    {
        storeEvent( STOP, 1, 1 ) ;
        recordingDevice = idle ;
    }
}

Event EventHandler::getEvent()
{
    Event localEvent ;
    byte error ;

    if( eepromType == INTERNAL_EEPROM ) {            EEPROM.get( eeAddress, localEvent ) ; }
    else                                { error = i2cEeprom.get( eeAddress, localEvent ) ; }
    
    //if(notifyI2cErrror) notifyI2cErrror( error ) ;

    if( displayGetMemory ) displayGetMemory( eeAddress ) ;

    eeAddress += sizeof( localEvent ) ;            // increase EEPROM address for next event ;

    return localEvent ;
}


void EventHandler::startPlaying() 
{
    if( recordingDevice == idle )
    {
        eeAddress = beginAddress ;
        event = getEvent() ;                                                    // should load the start event
        event.time2nextEvent = 1 ;                                              // be sure it is not '0'
        prevTime = millis() ;
        recordingDevice = playing ;
    }
    else if( recordingDevice == finishing )
    {
        recordingDevice = playing ;
    }
}
void EventHandler::stopPlaying() 
{
    if( recordingDevice == finishing )
    {
        recordingDevice = idle ;
    }
    else if( recordingDevice == playing )
    {
        recordingDevice = finishing ;
    }
}

void EventHandler::resetProgram() 
{
    recordingDevice = idle ;
}


void EventHandler::storeEvent( uint8 _data1, uint16 _data2, uint8 _data3 )
{
    if( recordingDevice != recording ) return ;

    Event     localEvent ;
    uint32    currTime = millis() ;

    localEvent.data1 = _data1 ;
    localEvent.data2 = _data2 ;
    localEvent.data3 = _data3 ;

    if(      _data1 == FEEDBACK )                    { localEvent.time2nextEvent = 0 ; }                       // feedback has 0 time -> wait until sensor is made...
    else if( _data1 == START ||  _data1 == STOP )    { localEvent.time2nextEvent = 1 ; }   
    else                                             { localEvent.time2nextEvent = currTime - prevTime ; }

    prevTime = currTime ;
    byte error ;

    if( eepromType == INTERNAL_EEPROM ) {            EEPROM.put( eeAddress, localEvent ) ; }
    else                                { error = i2cEeprom.put( eeAddress, localEvent ) ; }
    
    //if(notifyI2cErrror)                         notifyI2cErrror( error ) ; gets me error 4 which is kinda annoying, better to turn this one off
    if( displayStoreMemory )                    displayStoreMemory( eeAddress ) ;
    if( displayFreeMemory  )                    displayFreeMemory( ( (0x2000 - eeAddress - beginAddress) / sizeof( localEvent ) ) ) ;
    if( eeAddress == maxAddress && memoryFull ) memoryFull() ;

    eeAddress += sizeof( localEvent ) ;            // increase EEPROM address for next event ;
}


void EventHandler::sendFeedbackEvent( uint16 number )
{
    if( recordingDevice == playing || recordingDevice == finishing )
    {
        newSensor = number ;
    }
}

void EventHandler::update()
{
    uint32 currTime = millis() ;

    if( (recordingDevice == playing || recordingDevice == finishing )
    &&  (currTime - prevTime) >= event.time2nextEvent )
    {

        if( event.time2nextEvent == 0 )                                         
        {
            if( newSensor == event.data2 ) event.time2nextEvent = 1 ; // MAY CONTAIN BUG?? VERIVIED, START EVENT MAY NOT BE OK
            return ;
        }
                                       //    8bit         16bit        8bit       // for here and now, data1, is type, data2 is address and data 3 just data.
        if( notifyEvent ) notifyEvent( event.data1, event.data2, event.data3 ) ;

        prevTime = currTime ;
        
        if( event.data1 == STOP )
        {
            if( recordingDevice == finishing )
            {
                recordingDevice = idle ;
                if( notifyEvent ) notifyEvent( STOP, 1, 1 ) ;
                return ;
            }
            else
            {
                recordingDevice = idle ;
                startPlaying() ;
                return ;
            }
        }

        event = getEvent() ;
        newSensor = 0 ;
    }
}

uint8 EventHandler::getState()
{
    return recordingDevice ;                                                    // returns current state
}

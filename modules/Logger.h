#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

/*
 * Logger — expression watcher for development and testing.
 *
 * All name strings live in flash only. The macros apply F() automatically —
 * just pass a plain string literal, it never touches SRAM.
 *
 *   SUBSCRIBE_BOOL( expr, name )   — fires notifyLog on every 0/1 change.
 *   SUBSCRIBE_VAL(  expr, name )   — fires on change at the shared tick rate.
 *   UNSUBSCRIBE( name )            — remove watcher by name.
 *
 * All VALUE watchers share one tick timer. Call SetInterval(ms) to change
 * the rate (default 100 ms = 10 Hz). BOOL watchers are unaffected — they
 * always fire immediately on any change.
 *
 * Example:
 *
 *   logger.SUBSCRIBE_BOOL( digitalRead( pin4 ),  "pin4"  ) ;
 *   logger.SUBSCRIBE_BOOL( myFlag,               "flag"  ) ;
 *   logger.SUBSCRIBE_VAL(  encoder.getCount(),   "enc"   ) ;
 *   logger.SUBSCRIBE_VAL(  analogRead( A0 ),     "pot"   ) ;
 *   logger.SetInterval( 50 ) ;   // switch to 20 Hz
 *
 *   void notifyLog( const __FlashStringHelper* name, uint32_t value ) {
 *       Serial.print( name ) ; Serial.print( F(" -> ") ) ; Serial.println( value ) ;
 *   }
 *
 *   logger.UNSUBSCRIBE( "enc" ) ;
 */

#define SUBSCRIBE_BOOL(expr, name)   SubscribeBool_( []()->uint8_t { return (uint8_t)(expr)  ; }, F(name) )
#define SUBSCRIBE_VAL(expr, name)    SubscribeVal_(  []()->uint32_t{ return (uint32_t)(expr) ; }, F(name) )
#define UNSUBSCRIBE(name)            Unsubscribe_( F(name) )

class Logger
{
public:
    Logger() ;

    void SubscribeBool_( uint8_t(*fn)(),  const __FlashStringHelper* name ) ;
    void SubscribeVal_(  uint32_t(*fn)(), const __FlashStringHelper* name ) ;
    void Unsubscribe_(   const __FlashStringHelper* name ) ;

    void SetInterval( uint32_t ms ) ;
    void Log() ;

private:
    struct Watcher
    {
        enum Type : uint8_t { BOOL, VALUE } type ;
        union {
            uint8_t  (*boolFn)() ;
            uint32_t (*valFn)()  ;
        } fn ;
        const __FlashStringHelper* name ;
        uint32_t    lastValue ;
    } ;

    Watcher*    watchers     = nullptr ;
    uint8_t     nWatchers    = 0 ;
    uint32_t    tickInterval = 100 ;
    uint32_t    lastTick     = 0 ;

    void    AddWatcher( Watcher w ) ;
    void    RemoveAt(   uint8_t index ) ;
} ;

extern void notifyLog( const __FlashStringHelper* name, uint32_t value ) __attribute__((weak)) ;

#endif

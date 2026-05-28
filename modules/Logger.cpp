#include "Logger.h"

Logger::Logger() {}

// ─── private ────────────────────────────────────────────────────────────────

void Logger::AddWatcher( Watcher w )
{
    Watcher* n = new Watcher[nWatchers + 1] ;
    if( !n ) return ;

    for( uint8_t i = 0 ; i < nWatchers ; i++ )
        n[i] = watchers[i] ;

    n[nWatchers] = w ;

    delete[] watchers ;
    watchers = n ;
    nWatchers++ ;
}

void Logger::RemoveAt( uint8_t index )
{
    if( nWatchers == 0 ) return ;

    uint8_t newCount = nWatchers - 1 ;

    if( newCount == 0 )
    {
        delete[] watchers ;
        watchers  = nullptr ;
        nWatchers = 0 ;
        return ;
    }

    Watcher* n = new Watcher[newCount] ;
    if( !n ) return ;

    uint8_t j = 0 ;
    for( uint8_t i = 0 ; i < nWatchers ; i++ )
    {
        if( i == index ) continue ;
        n[j++] = watchers[i] ;
    }

    delete[] watchers ;
    watchers  = n ;
    nWatchers = newCount ;
}

// ─── public ─────────────────────────────────────────────────────────────────

void Logger::SubscribeBool_( uint8_t(*fn)(), const __FlashStringHelper* name )
{
    Watcher w ;
    w.type      = Watcher::BOOL ;
    w.fn.boolFn = fn ;
    w.name      = name ;
    w.lastValue = 0xFF ;        // force fire on first read
    AddWatcher( w ) ;
}

void Logger::SubscribeVal_( uint32_t(*fn)(), const __FlashStringHelper* name )
{
    Watcher w ;
    w.type      = Watcher::VALUE ;
    w.fn.valFn  = fn ;
    w.name      = name ;
    w.lastValue = 0xFFFFFFFF ;  // force fire on first read
    AddWatcher( w ) ;
}

void Logger::Unsubscribe_( const __FlashStringHelper* name )
{
    // F() literals in the same translation unit share the same flash address.
    for( uint8_t i = 0 ; i < nWatchers ; i++ )
    {
        if( watchers[i].name == name )
        {
            RemoveAt( i ) ;
            return ;
        }
    }
}

void Logger::SetInterval( uint32_t ms )
{
    tickInterval = ms ;
}

void Logger::Log()
{
    if( !watchers ) return ;

    bool tick = ( millis() - lastTick >= tickInterval ) ;
    if( tick ) lastTick = millis() ;

    for( uint8_t i = 0 ; i < nWatchers ; i++ )
    {
        Watcher& w = watchers[i] ;

        if( w.type == Watcher::BOOL )
        {
            uint32_t val = w.fn.boolFn() ;
            if( val != w.lastValue )
            {
                w.lastValue = val ;
                notifyLog( w.name, val ) ;
            }
        }
        else if( tick )
        {
            uint32_t val = w.fn.valFn() ;
            if( val != w.lastValue )
            {
                w.lastValue = val ;
                notifyLog( w.name, val ) ;
            }
        }
    }
}

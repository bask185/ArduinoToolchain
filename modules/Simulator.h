#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <Arduino.h>

/*
 * Simulator — sequential event runner for development and testing.
 *
 * All message strings live in flash only. ADD applies F() automatically —
 * just pass a plain string literal, it never touches SRAM.
 * Pass nullptr as message for silent steps.
 *
 * Events are numbered from 1 in the order they are registered.
 * Call Run() every loop() iteration. Call Reset() to replay from event 1.
 *
 *   ADD(  interval, msg, stmt )            — wait ms, print msg, execute stmt.
 *   STEP( interval, stmt )                 — silent step, no message.
 *   WAIT( expr )                           — hold until expr returns 1.
 *   JUMP( target )                         — unconditional jump to event <target>.
 *   CJUMP( expr, onFalse, onTrue )         — conditional jump.
 *   TRANSITION( cond, target )             — hold until cond, then jump to target.
 *   WAIT_SERIAL_COMMAND( "cmd" )           — hold until exact string received over Serial.
 *
 * MTRANSITION — requires #define TRANSITIONS_ENABLED (see below):
 *   MTRANSITION( c1,t1, c2,t2 )               — 2-way branch
 *   MTRANSITION( c1,t1, c2,t2, c3,t3 )        — 3-way branch
 *   MTRANSITION( c1,t1, c2,t2, c3,t3, c4,t4 ) — 4-way branch
 *   Holds at this step, evaluates conditions each loop() in order,
 *   jumps to the first matching target. Adds 13 bytes per SimEvent.
 *
 * Example:
 *
 *   sim.ADD(  100,  "hold button", digitalWrite( configPin, LOW  ) ) ;  // 1
 *   sim.ADD(  3500, "release",     digitalWrite( configPin, HIGH ) ) ;  // 2
 *   sim.WAIT( digitalRead( somePin ) == HIGH ) ;                        // 3
 *   sim.ADD(  200,  "send DCC",    notifyDccAccTurnoutOutput(5,1,1) ) ; // 4
 *   sim.ADD(  0,    nullptr,       counter++ ) ;                        // 5
 *   sim.CJUMP( counter < 5, 7, 2 ) ;                                   // 6
 *   sim.ADD(  500,  "done",        Serial.println( F("finished") ) ) ;  // 7
 */

// #define TRANSITIONS_ENABLED   // uncomment to enable MTRANSITION support (adds 13 bytes per SimEvent)

// ─── core macros ─────────────────────────────────────────────────────────────

#define ADD(interval, msg, ...)    add_(  interval, F(msg), []{ __VA_ARGS__ ; } )  // msg stored in flash only
#define STEP(interval, ...)        add_(  interval, nullptr, []{ __VA_ARGS__ ; } ) // silent — no message
#define WAIT(cond)                 wait_(       []()->uint8_t{ return (uint8_t)(cond) ; } )
#define JUMP(target)               jump_(       target )
#define CJUMP(cond, a, b)          cjump_(      []()->uint8_t{ return (uint8_t)(cond) ; }, a, b )
#define TRANSITION(cond, target)   transition_( []()->uint8_t{ return (uint8_t)(cond) ; }, target )

#define WAIT_SERIAL_COMMAND(cmd)   wait_( []()->uint8_t {                   \
    static char    buf[16] = {} ;                                           \
    static uint8_t idx     = 0  ;                                           \
    while( Serial.available() ) {                                           \
        char c = Serial.read() ;                                            \
        if( c == '\n' || c == '\r' ) {                                      \
            buf[idx] = '\0' ; idx = 0 ;                                     \
            if( strcmp( buf, cmd ) == 0 ) return 1 ;                        \
        } else if( idx < 15 ) { buf[idx++] = c ; }                         \
    }                                                                       \
    return 0 ;                                                              \
} )

// ─── MTRANSITION macros (compiled only when TRANSITIONS_ENABLED) ─────────────

#ifdef TRANSITIONS_ENABLED

#define _SIM_L(cond)  []()->uint8_t{ return (uint8_t)(cond) ; }

#define TRANSITION2(c1,t1, c2,t2)                               mtransition_( _SIM_L(c1),t1, _SIM_L(c2),t2 )
#define TRANSITION3(c1,t1, c2,t2, c3,t3)                       mtransition_( _SIM_L(c1),t1, _SIM_L(c2),t2, _SIM_L(c3),t3 )
#define TRANSITION4(c1,t1, c2,t2, c3,t3, c4,t4)                mtransition_( _SIM_L(c1),t1, _SIM_L(c2),t2, _SIM_L(c3),t3, _SIM_L(c4),t4 )

#define _MTRANS_DISPATCH(_1,_2,_3,_4,_5,_6,_7,_8, N, ...)  N
#define MTRANSITION(...)  _MTRANS_DISPATCH(__VA_ARGS__, TRANSITION4,TRANSITION4,TRANSITION3,TRANSITION3,TRANSITION2,TRANSITION2,TRANSITION2,TRANSITION2)(__VA_ARGS__)

#endif

// ─────────────────────────────────────────────────────────────────────────────

class Simulator
{
public:
    Simulator() ;

    void add_(        uint32_t interval, const __FlashStringHelper* message, void(*action)() ) ;
    void add_(        uint32_t interval, decltype(nullptr),                  void(*action)() ) ;
    void wait_(       uint8_t(*condition)() ) ;
    void jump_(       uint8_t target ) ;
    void cjump_(      uint8_t(*condition)(), uint8_t onFalse, uint8_t onTrue ) ;
    void transition_( uint8_t(*condition)(), uint8_t target ) ;

#ifdef TRANSITIONS_ENABLED
    void mtransition_( uint8_t(*c1)(), uint8_t t1,
                       uint8_t(*c2)(), uint8_t t2 ) ;
    void mtransition_( uint8_t(*c1)(), uint8_t t1,
                       uint8_t(*c2)(), uint8_t t2,
                       uint8_t(*c3)(), uint8_t t3 ) ;
    void mtransition_( uint8_t(*c1)(), uint8_t t1,
                       uint8_t(*c2)(), uint8_t t2,
                       uint8_t(*c3)(), uint8_t t3,
                       uint8_t(*c4)(), uint8_t t4 ) ;
#endif

    void Run() ;
    void Reset() ;

private:
    struct SimEvent
    {
        enum Type : uint8_t { TIMED, WAIT, JUMP, CJUMP, TRANSITION
#ifdef TRANSITIONS_ENABLED
            , MTRANSITION
#endif
        } type ;
        uint32_t                    interval ;
        uint8_t                     (*condition)() ;
        const __FlashStringHelper*  message ;
        void                        (*action)() ;
        uint8_t                     targetA ;
        uint8_t                     targetB ;
#ifdef TRANSITIONS_ENABLED
        uint8_t  (*branchCond[4])() ;
        uint8_t    branchTarget[4] ;
        uint8_t    nBranches ;
#endif
    } ;

    SimEvent*   events   = nullptr ;
    uint8_t     nEvents  = 0 ;
    uint8_t     current  = 0 ;
    uint32_t    lastTime = 0 ;

    void AddEvent( SimEvent e ) ;
    void JumpTo(   uint8_t target ) ;
} ;

#endif

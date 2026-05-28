#include "Simulator.h"

Simulator::Simulator() {}

// ─── private ────────────────────────────────────────────────────────────────

void Simulator::AddEvent( SimEvent e )
{
    SimEvent* n = new SimEvent[nEvents + 1] ;
    if( !n ) return ;

    for( uint8_t i = 0 ; i < nEvents ; i++ )
        n[i] = events[i] ;

    n[nEvents] = e ;

    delete[] events ;
    events  = n ;
    nEvents++ ;
}

void Simulator::JumpTo( uint8_t target )
{
    if( target < 1 || target > nEvents ) return ;
    current  = target - 1 ;
    lastTime = millis() ;
}

// ─── public ─────────────────────────────────────────────────────────────────

void Simulator::add_( uint32_t interval, const __FlashStringHelper* message, void(*action)() )
{
    AddEvent( { SimEvent::TIMED, interval, nullptr, message, action, 0, 0 } ) ;
}

void Simulator::add_( uint32_t interval, decltype(nullptr), void(*action)() )
{
    AddEvent( { SimEvent::TIMED, interval, nullptr, nullptr, action, 0, 0 } ) ;
}

void Simulator::wait_( uint8_t(*condition)() )
{
    AddEvent( { SimEvent::WAIT, 0, condition, nullptr, nullptr, 0, 0 } ) ;
}

void Simulator::jump_( uint8_t target )
{
    AddEvent( { SimEvent::JUMP, 0, nullptr, nullptr, nullptr, target, 0 } ) ;
}

void Simulator::cjump_( uint8_t(*condition)(), uint8_t onFalse, uint8_t onTrue )
{
    AddEvent( { SimEvent::CJUMP, 0, condition, nullptr, nullptr, onFalse, onTrue } ) ;
}

void Simulator::transition_( uint8_t(*condition)(), uint8_t target )
{
    AddEvent( { SimEvent::TRANSITION, 0, condition, nullptr, nullptr, target, 0 } ) ;
}

#ifdef TRANSITIONS_ENABLED

void Simulator::mtransition_( uint8_t(*c1)(), uint8_t t1,
                               uint8_t(*c2)(), uint8_t t2 )
{
    SimEvent e = { SimEvent::MTRANSITION, 0, nullptr, nullptr, nullptr, 0, 0 } ;
    e.branchCond[0] = c1 ; e.branchTarget[0] = t1 ;
    e.branchCond[1] = c2 ; e.branchTarget[1] = t2 ;
    e.nBranches = 2 ;
    AddEvent( e ) ;
}

void Simulator::mtransition_( uint8_t(*c1)(), uint8_t t1,
                               uint8_t(*c2)(), uint8_t t2,
                               uint8_t(*c3)(), uint8_t t3 )
{
    SimEvent e = { SimEvent::MTRANSITION, 0, nullptr, nullptr, nullptr, 0, 0 } ;
    e.branchCond[0] = c1 ; e.branchTarget[0] = t1 ;
    e.branchCond[1] = c2 ; e.branchTarget[1] = t2 ;
    e.branchCond[2] = c3 ; e.branchTarget[2] = t3 ;
    e.nBranches = 3 ;
    AddEvent( e ) ;
}

void Simulator::mtransition_( uint8_t(*c1)(), uint8_t t1,
                               uint8_t(*c2)(), uint8_t t2,
                               uint8_t(*c3)(), uint8_t t3,
                               uint8_t(*c4)(), uint8_t t4 )
{
    SimEvent e = { SimEvent::MTRANSITION, 0, nullptr, nullptr, nullptr, 0, 0 } ;
    e.branchCond[0] = c1 ; e.branchTarget[0] = t1 ;
    e.branchCond[1] = c2 ; e.branchTarget[1] = t2 ;
    e.branchCond[2] = c3 ; e.branchTarget[2] = t3 ;
    e.branchCond[3] = c4 ; e.branchTarget[3] = t4 ;
    e.nBranches = 4 ;
    AddEvent( e ) ;
}

#endif

// ─── Run / Reset ────────────────────────────────────────────────────────────

void Simulator::Run()
{
    if( !events || current >= nEvents ) return ;

    SimEvent& e = events[current] ;

    switch( e.type )
    {
    case SimEvent::TIMED:
        if( millis() - lastTime >= e.interval )
        {
            lastTime = millis() ;
            if( e.message ) Serial.println( e.message ) ;
            if( e.action  ) e.action() ;
            current++ ;
        }
        break ;

    case SimEvent::WAIT:
        if( e.condition() )
            current++ ;
        break ;

    case SimEvent::JUMP:
        JumpTo( e.targetA ) ;
        break ;

    case SimEvent::CJUMP:
        JumpTo( e.condition() ? e.targetB : e.targetA ) ;
        break ;

    case SimEvent::TRANSITION:
        if( e.condition() )
            JumpTo( e.targetA ) ;
        break ;

#ifdef TRANSITIONS_ENABLED
    case SimEvent::MTRANSITION:
        for( uint8_t i = 0 ; i < e.nBranches ; i++ )
        {
            if( e.branchCond[i]() )
            {
                JumpTo( e.branchTarget[i] ) ;
                break ;
            }
        }
        break ;
#endif
    }
}

void Simulator::Reset()
{
    current  = 0 ;
    lastTime = millis() ;
}

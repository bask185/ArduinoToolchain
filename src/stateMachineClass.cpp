#include "stateMachineClass.h"

/**
 * @brief creates an instance of the state machine class. Used to handle the flow
 */
StateMachine::StateMachine() 
{
    state = 0 ;
    runOnce = true ;
    exitFlag = false ;
}

/**
 * @brief manually set a state
 *
 * @param The state
 *
 * @return N/A
 */
void StateMachine::setState( uint8_t _state )
{
    state = _state ;
    runOnce = true ;
}

/**
 * @brief Lets the entry state to run again. May be usefull from time to time
 *
 * @param N/A
 *
 * @return N/A
 */
void StateMachine::reboot()
{
    runOnce = true ;
}

/**
 * @brief  returns the current state variable
 *
 * @param N/A
 *
 * @return current state
 */
uint8_t StateMachine::getState()
{
    return state ;
}

/**
 * @brief function to handle the one time condition for the entry states
 *
 * @param N/A
 *
 * @return true or false
 */
uint8_t StateMachine::entryState()
{
    uint8_t retVal = runOnce ;
    runOnce = 0 ;
    return retVal ;
}

/**
 * @brief function to continously handle the on state, could be a macro
 *
 * @param N/A
 *
 * @return 1
 */
uint8_t StateMachine::onState()
{
    return 1 ;
}

/**
 * @brief function to handle the one time condition for the exit states
 *
 * @param N/A
 *
 * @return true or false
 */
uint8_t StateMachine::exitState()
{
    return exitFlag ; ;
}

/**
 * @brief Ensures the exit state of a state function will be executed
 *
 * @param N/A
 *
 * @return N/A
 */
void StateMachine::exit()
{
    exitFlag = true ;
}

/**
 * @brief sends signal to state machine if current state function is finished or not
 *
 * @param N/A
 *
 * @return exit flag
 */
uint8_t StateMachine::endState( )
{
    return exitFlag ;
}


/**
 * @brief sets a timestamp which can be used for timing within state functions
 *
 * @param timeout time in ms
 *
 * @return N/A
 */
void StateMachine::setTimeout( uint32_t time2run )
{
    prevTime = millis() ;
    interval = time2run ;
}

/**
 * @brief Monitors if the time in set by setTime() has passed 
 *
 * @param N/A
 *
 * @return true or false
 */
uint8_t StateMachine::timeout()
{
    if( millis() - prevTime >= interval ) return 1 ;
    
    return 0 ;
}

/**
 * @brief Transisition from current state to the next state
 *
 * @param _state new state to execute
 * @param _interval when to execute new state
 *
 * @return N/A
 */
void StateMachine::nextState( uint8_t _state, uint32_t _interval ) 
{
    exitFlag = 0 ;
    runOnce = 1 ;
    if( _interval )
    {
        enabled = 0 ;
        prevTime = millis() ;
        interval = _interval ;
    }
    state = _state ;
}

/**
 * @brief monitors the interval when there is an interval set between states
 *
 * @param N/A
 *
 * @return enabled flag
 */
uint8_t StateMachine::run()
{
    if( enabled == 0 )
    {
        if( millis() - prevTime >= interval )
        {
            enabled = 1 ;
        }
    }
    return enabled ;
}

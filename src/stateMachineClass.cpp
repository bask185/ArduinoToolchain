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
void StateMachine::setState( uint8 _state )
{
    state = _state ;
    runOnce = true ;
    exitFlag = false ;
}

/**
 * @brief Lets the entry state to run again in a certain amount of time. Can be usefull from time to time
 *
 * @param delay delayed execution if desired
 *
 * @return N/A
 */
void StateMachine::reboot( uint32 _interval )
{
    runOnce = true ;
    if( _interval )
    {
        enabled = 0 ;
        prevTime = millis() ;
        interval = _interval ;
    }
}

/**
 * @brief  returns the current state variable
 *
 * @param N/A
 *
 * @return current state
 */
uint8 StateMachine::getState()
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
uint8 StateMachine::entryState()
{
    uint8 retVal = runOnce ;
    runOnce = 0 ;
    timeOutSet = false ;
    return retVal ;
}

/**
 * @brief function to continously handle the on state, could be a macro
 *
 * @param N/A
 *
 * @return 1
 */
uint8 StateMachine::onState()
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
uint8 StateMachine::exitState()
{
    return exitFlag ;
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
uint8 StateMachine::endState( )
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
void StateMachine::setTimeout( uint32 time2run )
{
    prevTime = millis() ;
    interval = time2run ;
    timeOutSet = false ;
}

/**
 * @brief Monitors if the time in set by setTime() has passed 
 *
 * @return true or false
 */
uint8 StateMachine::timeout()
{
    if( (millis() - prevTime) >= interval && timeOutSet == false )
    {
        timeOutSet = true ; 
        return 1 ;
    }
    
    return 0 ;
}

/**
 * @brief returns true if an timeout error has happened
 *
 * @return true or false
 */
uint8 StateMachine::timeoutError()
{
    return timeOutSet ;
}

/**
 * @brief Transisition from current state to the next state
 *
 * @param _state new state to execute
 * @param _interval when to execute new state
 *
 * @return N/A
 */
void StateMachine::nextState( uint8 _state, uint32 _interval ) 
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
uint8 StateMachine::run()
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

/**
 * @brief Called from within an if-statement, this method can be used for
 * repeating parts of code with a certain interval
 *
 * @param N/A
 *
 * @return enabled flag
 */
uint8 StateMachine::repeat( uint32 _interval )
{
    if( millis() - prevTime >= _interval )
    {
        prevTime = millis() ;
        return true ;
    }
    return false ;
}

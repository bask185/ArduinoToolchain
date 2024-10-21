/*
 * Copyright (C) 2024 Sebastiaan Knippels, Train-Science
 *
 * To the extent possible under law, the person who associated CC0 with this work
 * has waived all copyright and related or neighboring rights to this work.
 *
 * This work is published from: The Netherlands
 *
 * You can copy, modify, distribute and perform the work, even for commercial purposes,
 * all without asking permission. See the full license for details at:
 * https://creativecommons.org/publicdomain/zero/1.0/
 */

#include "errorHandler.h"

ErrorHandler::ErrorHandler() : errorCount(0), head(0), tail(0), errorPrinted(false) {}

bool ErrorHandler::pushError(const char* errorMessage, bool (*resolver)()) 
{
    if (errorCount >= MAX_ERRORS) 
	{
        return false ; 
    }

    strncpy(errorQueue[tail].message, errorMessage, sizeof(errorQueue[tail].message) - 1) ;
    errorQueue[tail].message[31] = '\0' ;
    errorQueue[tail].resolver = resolver ;

    tail = (tail + 1) % MAX_ERRORS ;
    errorCount++ ;
    
    return true ;
}

void ErrorHandler::processErrors() 
{
    if (errorCount == 0) 
	{
        return ; 
    }

    if (!errorPrinted) 
	{
        if( printError )
        {
            printError( errorQueue[head].message ) ; // instead of using Serial from arduino, I use a callback to keep it plain C++
        }
    
        errorPrinted = true ;
    }

    if (errorQueue[head].resolver()) 
	{
        head = (head + 1) % MAX_ERRORS ;
        errorCount-- ;
        errorPrinted = false ;
    }
}

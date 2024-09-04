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

#include "Trigger.h"

R_trigger::R_trigger()
{
}

void R_trigger::update( uint8 IN )
{
    Q = 0 ;

    if( IN != old )
    {
        Q   = IN ;
        old = IN ;
    }
} 

F_trigger::F_trigger()
{
}

void F_trigger::update( uint8 IN )
{
    Q = 0 ;

    if( IN != old )
    {
        Q   = IN ^ 1 ;
        old = IN ;
    }
} 
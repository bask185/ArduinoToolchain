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

#include "Encoder.h"

Encoder::Encoder( uint8_t _clk, uint8_t _data, uint8_t _sw )
{
	clkPin = _clk ;
	dataPin = _data ;
	swPin = _sw ;
}

void Encoder::begin()
{
	clk.begin( clkPin ) ;			// initalize debounce objects
	data.begin( dataPin ) ;
	sw.begin( swPin ) ;
}

void Encoder::update()
{
	clk.debounce() ;
	data.debounce() ;
	sw.debounce() ;
}

uint8_t Encoder::getState()
{
	uint8_t clkState = clk.readInput(); 
	uint8_t dataState = data.readInput(); 
	uint8_t swState = sw.readInput();


	if( clkState == RISING )
	{
		if( dataState == HIGH ) return   leftPulse ;
		else      	            return  rightPulse ;
	}
	else return swState ;
}
	



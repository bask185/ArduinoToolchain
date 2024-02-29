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
	



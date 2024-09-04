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

#include "i2cEeprom.h"
#include <Wire.h>

I2cEeprom::I2cEeprom() {;}
I2cEeprom::I2cEeprom( uint8 addr )
{
    i2cAddress = addr ;
}

void I2cEeprom::begin( uint8 addr )
{
	i2cAddress = addr ;
}

uint8 I2cEeprom::get_n( uint16 eeAddress, void * src, int size )
{
    uint8 * ptr = reinterpret_cast<uint8 const *>(src) ;

    Wire.beginTransmission( i2cAddress ) ;
    Wire.write( highByte(eeAddress) ) ;
    Wire.write(  lowByte(eeAddress) ) ;
    uint8 state = Wire.endTransmission() ;

    Wire.requestFrom( i2cAddress, size) ;

    for( int i = 0 ; i < size ; i ++ )
    {
        uint8 val = Wire.read() ; 
        *ptr = val ; 
        ptr ++ ;
    }

    return state ;
}

uint8 I2cEeprom::put_n( uint16 eeAddress, void * src, int size )
{
    uint8 * ptr = reinterpret_cast<uint8 *>(src);

    Wire.beginTransmission( i2cAddress ) ;
    Wire.write( highByte(eeAddress) ) ;
    Wire.write( lowByte(eeAddress) ) ;

    for ( int i = 0 ; i < size ; i++ )
    {
        uint8 val = *ptr++ ;
        Wire.write( val ) ;
    }
    return Wire.endTransmission() ;
}

void I2cEeprom::write( uint16 eeAddress, uint8 data )
{
	Wire.beginTransmission( i2cAddress ) ;
    Wire.write( highByte(eeAddress) ) ;
    Wire.write( lowByte(eeAddress) ) ;
	Wire.write( data ) ;
	Wire.endTransmission() ;
}

uint8 I2cEeprom::read( uint16 eeAddress )
{
	Wire.beginTransmission( i2cAddress ) ;
    Wire.write( highByte(eeAddress) ) ;
    Wire.write(  lowByte(eeAddress) ) ;
    Wire.endTransmission() ;

    Wire.requestFrom( i2cAddress, 1 ) ;
	return Wire.read() ;
}

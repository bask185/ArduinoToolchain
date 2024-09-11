#include "Transceiver.h"


Transceiver::Transceiver()
{
}

void Transceiver::sendMessage( Message *mess2send )
{
    uint8 len = mess2send->command & 0x0F ;
    if( len == 0x0F ) len = mess2send->payload[0] ;

    Serial.write( mess2send->command ) ;

    for( int i = 0 ; i < len ; i ++ )
    {
        Serial.write( mess2send->payload[i] ) ;

        // pinMode(13,OUTPUT ) ;
        // PORTB |= 1 << 5 ;
        // delay(150) ;
        // PORTB ^= 1 << 5 ;
        // delay(350) ;
    }
}

void Transceiver::sendAck()
{
    Serial.write( 0x70 ) ;
}

uint8 Transceiver::receive()
{
    if( Serial.available() == 0 ) return 0 ;

    uint8 b = Serial.read() ;

    switch( state )
    {
    case 0:
        mess.command = b ;
        len          = b & 0x0F ;
        byteCounter  = 0 ;
        //Serial.print("len: ");Serial.println(len);

        if(      len == 0x00 ) goto noFollowBytes ;
        else if( len == 0x0F ) state = 1 ; // extra LEN byte is following
        else                   state = 2 ;
        break ;

    case 1: // extra len byte
        len = b ;
        mess.payload[ byteCounter++ ] = b ;
        state ++ ;
        break ;

    case 2:
        mess.payload[ byteCounter ] = b ;
        //Serial.print("payload @");Serial.print(byteCounter); Serial.print(" = ");Serial.println(b,HEX);
        if( ++byteCounter == len ) goto noFollowBytes ;
        break ;

    noFollowBytes:
    case 3:
        if( messageReceived )
            messageReceived( &mess, len ) ;
        state = 0 ;
        return 1 ;
    }

    return 0 ;
}

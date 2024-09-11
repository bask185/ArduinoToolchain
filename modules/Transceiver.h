#include <Arduino.h>
#include "macros.h"

typedef struct mess
{
    uint8 command ;
    uint8 payload[32] ;
} Message ;

class Transceiver
{
public:
    Transceiver(/* args */);
    uint8   receive() ;
    //void    processMessage() ;
    void    sendMessage(Message *) ;
    void    sendAck() ;

private:
    Message mess ;
    uint8 state ;
    uint8 len ;
    uint8 byteCounter ;
} ;

extern void messageReceived( Message *, uint8 ) __attribute__ ((weak)) ; 




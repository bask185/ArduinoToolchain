/*
TODO
V repeat new instructions X amount of time 
V count how many new instructions are waiting and if needed reduce repeat amount. 
V add idle packets for unused slots
V add dynamic slot allocator (IDEA: every minute or so, increment a priority counter only if an address has been used
  in that minute OR decrement the number if it has not been used. if all slots are in use, the slot with the lowest priority number will be replaced.
  newly allocated slots start with a random number of lets say '10'.
V add the accesorries
V add the accesorries extended
V fix the directions (should be good, needs testing)
V ensure that F0 - F4 goes well
V ensure that speeds go well (swap 1 and 0 somewhere )

* implement loconet SLOT manager for throttles

V TEST automatic slot manager by hardcoding some instructions _> N.B. priority system seems to work
* TEST DCC via hardcoded instructions first test a loco, than test accessories. Also test DCC extended commands
* TEST loconet to set accessories
* TEST OVER CURRENT MECHANISM (use lower current settings and 1A source or so)

*/

// #define TIMER1 NOTE TIMER 1 has mistake
#define TIMER2

#if defined TIMER1
#define TCCR_A      TCCR1A
#define TCCR_B      TCCR1B
#define TCNT        TCNT1
#define CS_0        CS10
#define CS_1        CS11
#define CS_2        CS12
#define TIMSK       TIMSK1
#define ENABLE_BIT  OCIE1A
#define OCRA        OCR1A
#define TIMER_COMPA_vect   TIMER1_COMPA_vect
#define WGM_1       WGM11

const int   DCC_ZERO_BIT =  1855 ; // 116us prescaler
const int   DCC_ONE_BIT  =   927 ; // 58us 

#elif defined TIMER2
#define TCCR_A      TCCR2A
#define TCCR_B      TCCR2B
#define TCNT        TCNT2
#define CS_0        CS20
#define CS_1        CS21
#define CS_2        CS22
#define TIMSK       TIMSK2
#define OCRA        OCR2A
#define ENABLE_BIT  OCIE2A
#define TIMER_COMPA_vect   TIMER2_COMPA_vect
#define WGM_1       WGM21

const int   DCC_ZERO_BIT =  1855/8 ; // 116us 8x prescaler
const int   DCC_ONE_BIT  =   927/8 ; // 58us 

#else
#error NO TIMER DEFINED
#endif

// HEADER FILES
#include "DCC.h"
#include "macros.h"
#include "slot.h"
#include "io.h" 
#include "stateMachineClass.h"

uint32 beginTime ;

StateMachine sm ;
// CONSTANTS
const int   maxPreAmble         =   17 ; // seems to be adjusted in RCN210 or 211..
const int   _128_STEPS          = 0x3F ;
const int   LONG_ADDRESS        = 0xC0 ;
const int   nAccessoryRepeats   =    6 ;
const int   nAccessories        =   10 ; // size of accessory buffer

enum packetTypes 
{
    speed_packet ,
    F0_F4_packet ,
    F5_F8_packet ,
    F9_F12_packet ,
    F13_F20_packet ,
    F21_F28_packet ,
    F29_F36_packet ,
    accessoryPacket ,
    accessoryExtendedPacket ,
} ;

enum trainCommands
{
    FORWARD             = 0b011  << 5,
    DRIVE               = 0b010  << 5,
    F0_F4_COMMAND       = 0B100  << 5, 
    F5_F8_COMMAND       = 0B1011 << 4,
    F9_F12_COMMAND      = 0B1010 << 4,
    F13_F20_COMMAND     = 0b11011110 ,
    F21_F28_COMMAND     = 0b11011111 , 
    F29_F36_COMMAND     = 0b11011000 ,
    // F37_F44    = 0b11011001 , Train-science does not yet believe in this BS who needs more than F36 functions
    // F45_F52    = 0b11011010 , 
    // F53_F60    = 0b11011011 , 
    // F61_F68    = 0b11011100 , 
} ;   

enum states
{
    selectNewPacketType ,
    assemblePacket ,
    awaitPacketSent ,
} ;

enum ISRstates
{
    preAmble,
    sendZero,
    sendByte,
    sendOne,
    theEnd,
} ;

// VARIABLES

struct
{
    uint8 data[10] ;
    uint8 length ;                    // these variable keeps track of ammount of bytes in FIFO buffer
} buffer[2] ;

uint8   *ptr ;
uint8   packetType = speed_packet, sendPacket_state = 0, Bit ;    // for DCC packets
uint8   currentAddres=0 ;    // must be int to prevent overflows during makeAddres()
uint8   newLocoInstructionFlag, state = assemblePacket ;
uint8   newLocoInstructionCounter ;
uint8   currentSlot ;
uint8   toSend ;
uint8   toFill ;
uint8   repeatCounter ;
uint8   currentAddress ;
uint8   someBit ;
uint8   repeatCount ;

struct accessories          // small buffer to store accessory commands in
{                           // if central receives more than 1 accessory instuction before the previous one is
    uint16  address : 15 ;  // clocked out, the instruction would otherwise be discarded.
    uint8   ext     : 1 ;
    uint8   state ;
} accessory[nAccessories];

uint8   accessoryIn  ;
uint8   accessoryOut ;
uint8   accessoryNow ; // retains address information during repeats


// CONTROL FUNCTIONS
enum instructions
{
    speedInstruction,
    setF1F4,
    clrF1F4,
    setF5F8,
    clrF5F8,
    headlightInstruction,
} ;


// CONTROL FUNCTIONS TO CONTROLL DCC CENTRAL WITH
void setAccessoryExt( uint16 address, uint8 state, uint8 ext )
{
    accessory[accessoryIn].address = address - 1 ;
    accessory[accessoryIn].state   =   state ;
    accessory[accessoryIn].ext     =     ext ;

    if( ++ accessoryIn == nAccessories ) accessoryIn = 0 ; // handle overflow of index
}

void setAccessoryExt( uint16 address, uint8 state ) // call above function and flag EXT
{
    setAccessoryExt( address, state, 1 ) ;
}

void setAccessory( uint16 address, uint8 state )    // call above function and flag NO EXT
{
    setAccessoryExt( address, state, 0 ) ;
}


void setRepeatAmount()
{
    uint8 waitingInstructions = 0 ;
    for (int i = 0; i < maxSlot; i++)
    {
        if( slot[i].newInstruction > 0 ) waitingInstructions ++ ;
    }

                                   repeatCount = 20 ;
    if( waitingInstructions > 10 ) repeatCount = 15 ;      // how more new packets are to be sent, how less they are repeated.
    if( waitingInstructions > 15 ) repeatCount = 10 ;
    if( waitingInstructions > 20 ) repeatCount =  6 ;
}

void toggleDir( uint16 address, uint8 speed ) // do I need a setDir instead? from loconet I may actually get a new absolute dir instead of a toggle..
{
    uint8 currentSlot = getSlot( address ) ;

    slot[currentSlot].speed ^= (1<<7) ;                 // valid for 126 speed steps. 28 not supported at this time.
    slot[currentSlot].newInstruction |= newSpeed ;      // set flag new instruction
    slot[currentSlot].set = 1 ;
    setRepeatAmount() ;
}

void setSpeed( uint16 address, uint8 speed )
{
    uint8 currentSlot = getSlot( address ) ;

    if( speed >= 1 ) speed ++ ; // skip E-stop and compensate

    uint8 dir = bitRead( slot[currentSlot].speed, 7 ) ; // fetch dir
    slot[currentSlot].speed = speed ;
    bitWrite( slot[currentSlot].speed, 7, dir ) ;       // set dir back

    slot[currentSlot].newInstruction |= newSpeed ;      // set flag new instruction
    slot[currentSlot].set = 1 ;
    setRepeatAmount() ;
}

void setFunction( uint16 address, uint8 Fx, uint8 state ) // note may directly insert the data from loconet.
{
    uint8 currentSlot = getSlot( address ) ;
    
    uint8 *bank ; // ALL SEEMS TO WORK HERE!!
            if( Fx >=  0 && Fx <=  4 ) { if( -- Fx == 255 )  Fx = 4 ;          // this compensates with the position of F0
                                        bank = &slot[currentSlot].F0_F4   ;            slot[currentSlot].newInstruction |= new_F0_F4   ; }
    else if( Fx >=  5 && Fx <=  8 ) { bank = &slot[currentSlot].F5_F8   ; Fx -=  5 ; slot[currentSlot].newInstruction |= new_F5_F8   ; }
    else if( Fx >=  9 && Fx <= 12 ) { bank = &slot[currentSlot].F9_F12  ; Fx -=  9 ; slot[currentSlot].newInstruction |= new_F9_F12  ; }
    else if( Fx >= 13 && Fx <= 20 ) { bank = &slot[currentSlot].F13_F20 ; Fx -= 13 ; slot[currentSlot].newInstruction |= new_F13_F20 ; }
    else if( Fx >= 21 && Fx <= 28 ) { bank = &slot[currentSlot].F21_F28 ; Fx -= 21 ; slot[currentSlot].newInstruction |= new_F21_F28 ; }
    else if( Fx >= 29 && Fx <= 36 ) { bank = &slot[currentSlot].F29_F36 ; Fx -= 29 ; slot[currentSlot].newInstruction |= new_F29_F36 ; }

    if( state ) *bank |=  (1<<Fx) ;
    else        *bank &= ~(1<<Fx) ;

    slot[currentSlot].set = 1 ;
    setRepeatAmount() ;
}





/******** STATE FUNCTIONS ********/
// in order of priotiry we check for:
// if an instructions is to be/being repeated
// if an accessory is set
// if a loco has has new instructions
// and otherwise we go do cyclic repeat instructions for locos
StateFunction( selectNewPacketType )
{
    uint8 i = 0 ;
    uint8 j = 0 ;

    if( repeatCounter )
    {   repeatCounter -- ;
        // keep doing current packet
        return 1 ;
    }

    if( accessoryIn != accessoryOut )       // an accessory has to be set..
    {
        if( accessory[accessoryOut].ext == 1 ) packetType = accessoryExtendedPacket ;
        else                                   packetType = accessoryPacket ;

        accessoryNow = accessoryOut ;
        repeatCounter = nAccessoryRepeats ;         // set the amount of repeats

        if( ++ accessoryOut == nAccessories )  accessoryOut = 0 ;    // handle overflow of index
        return 1 ;
    }


    // check for new locomotive instructions
    for( i = 0 ; i < maxSlot ; i++ )
    {
        if( slot[i].newInstruction > 0 )
        {
            currentSlot = i ;
            repeatCounter = repeatCount ;
            
            for( j = 0 ; j < 8 ; j ++ )     // we loop through speed packets all the way to F29-F36
            {
                uint8 mask = 1<<j ;

                if( slot[i].newInstruction &   mask )
                {   slot[i].newInstruction &= ~mask ; 
                  
                    packetType = j ; 
                    return 1 ;
                }
            }
        }
    }

    // nothing to repeat and no new instructions to send.
    // cyclic loco instructions
    if( ++ currentSlot >= maxSlot ) // loop through all slots.
    {      currentSlot = 0 ;

        // ALL ADDRESSES CYCLED, PICK NEXT PACKET TYPE
        if( ++ packetType == nPacketTypes )
        { 
            packetType = 0 ;
        }
    }
    // Serial.println("\r\ncyclic instruction: ") ;
    // printNumberln("currentSlot: ", currentSlot);
    // printNumberln("address: ", slot[currentSlot].address);
    // printNumberln("packetType: ", packetType);

    return 1 ;
}

StateFunction( assemblePacket )
{
    const int   speed = 0 ; // array indices
    uint8       index = 0 ;
            
    toFill ^= 1 ; // toggle to the other buffer to fill

    
/******   LOCO ADDRESS    *******/
    if( packetType < accessoryPacket )       // no accessory is due, go do locomotive
    {
        if( slot[currentSlot].address == 0 ) // trains with address = 0 are replaced by IDLE packets.
        {
            buffer[toFill].data[0] = 0xFF ;
            buffer[toFill].data[1] = 0x00 ;
            buffer[toFill].data[2] = 0xFF ;
            buffer[toFill].length  =    3 ;
            return true ;
        } 

        // LOAD LOCO ADDRESS
        if( slot[currentSlot].address > 127 )        // long address
        {   // load long address command and high byte of address
            buffer[toFill].data[index++] = LONG_ADDRESS | slot[currentSlot].address >> 8  ;
        }

        // load short address
        buffer[toFill].data[index++] = slot[currentSlot].address & 0xFF ;
    }

    switch(packetType)
    {
    /******    SPEED    *******/ 
    case speed_packet:
        // for 128 steps  
        buffer[toFill].data[index++] = _128_STEPS ;
        buffer[toFill].data[index++] = slot[currentSlot].speed ; // first bit contains direction
        
        /* 28 steps NOT YET IMPLEMENTED
        uint8 speedBits = slot[currentSlot].speed & 0b11111 ;
        uint8    dirBit = slot[currentSlot].speed >> 5 ;
        uint8      bit4 = bitRead( slot[currentSlot].speed, 0 ) ;
        uint8      bit0 = bitRead( slot[currentSlot].speed, 4 ) ;

        buffer[toFill].data[index++] = 
              DRIVE 
            | speedBits 
            | (dirBit << 5) ;
            |  bit0
            | (bit4<<4) ;  // NOTE VERIFY ME
        */
        break ;
    
    // following 3 function banks fit in single byte
    /******    FUNCTIONS F0 - F4    *******/    
    case F0_F4_packet:
        buffer[toFill].data[index++] = F0_F4_COMMAND | slot[currentSlot].F0_F4 ;
        break ; 
            
    /******    FUNCTIONS F5 - F8    *******/ 
    case F5_F8_packet: 
        buffer[toFill].data[index++] = F5_F8_COMMAND | slot[currentSlot].F5_F8 ;
        break ; 

    /******    FUNCTIONS F9 - F12    *******/
    case F9_F12_packet:
        buffer[toFill].data[index++] = F9_F12_COMMAND | slot[currentSlot].F9_F12 ;
        break ;

    // the higher functions need 2 bytes
    /******    FUNCTIONS F13 - F20    *******/
    case F13_F20_packet:
        buffer[toFill].data[index++] = F13_F20_COMMAND ;
        buffer[toFill].data[index++] = slot[currentSlot].F13_F20 ;
        break ;

    /******    FUNCTIONS F21 - F28    *******/
    case F21_F28_packet:
        buffer[toFill].data[index++] = F21_F28_COMMAND ;
        buffer[toFill].data[index++] = slot[currentSlot].F21_F28 ;
        break ; 

    /******    FUNCTIONS F29 - F36    *******/
    case F29_F36_packet:
        buffer[toFill].data[index++] = F29_F36_COMMAND ;
        buffer[toFill].data[index++] = slot[currentSlot].F29_F36 ;
        break ;

//  Paket: 1 0 A A - A A A A | 1 A A A - D A A R
//  Paket: 1 0 A A - A A A A | 0 A A A - 0 A A 1   EXT

//  Adresse: – – A7 A6 - A5 A4 A3 A2 – /A10 /A9 /A8 - – A1 A0 –
//  Adresse: – – A7 A6 - A5 A4 A3 A2 – /A10 /A9 /A8 - – A1 A0 – EXT

    case accessoryPacket:
    case accessoryExtendedPacket: 
        uint8  addressLow = accessory[accessoryNow].address ;
        uint8 addressHigh = accessory[accessoryNow].address >> 8 ;              //only A10, A9 and A8 are used
        uint8       state = accessory[accessoryNow].state ;
        uint8         ext = accessory[accessoryNow].ext ;

        buffer[toFill].data[index++] = 0x80 | addressLow >> 2 ;                 // Puts A7 -A2 in the upper byte with A2 @ bit 0

// NOTE POWER STATE/ACTIVE or D IS YET NOT PROCESSED. not sure if needed otherwise this function should be OKAY
        if( ext == 0 ) // CONVENTIONAL ACCESSORY
        { 
        buffer[toFill].data[index++] = 0x80 | ((  addressLow & 0b011 ) << 1 )   // A1 and A0 shifted one to the left
                                            | ((~addressHigh & 0b111 ) << 4 )   // A10, A9 and A8 all inverted and shifted 4 to the left (SEEMS TO BE OKAY)
                                            | (        state & 0b001        ) ; // sets the direction of the accessory
        }
        else           // EXTENDED ACCESSORY
        {
        buffer[toFill].data[index++] = 0x01 | ((  addressLow & 0b011 ) << 1 )   // A1 and A0 shifted one to the left
                                            | ((~addressHigh & 0b111 ) << 4 ) ; // invert A10, A9 and A8 and shift 4 to the left
        buffer[toFill].data[index++] = state ;
        }
        break ;
    }

    /***** CHECKSUM *******/
    
    uint8 checksum = 0 ;
    for( int i = 0 ; i < index ; i ++ )
    {
        checksum ^= buffer[toFill].data[i] ;
    }

    buffer[toFill].data[index++] = checksum ;
    buffer[toFill].length = index  ;

    return true ;
}

/*
Pakket: 10AAAAAA 1AAADAAR
Adres: – – A7 A6 - A5 A4 A3 A2 – /A10 /A9 /A8 - – A1 A0 –
De eerstgenoemde is vanwege compatibiliteit met bestaande controlecentra
Adres 4 = 1000-0001 1111-D00R. Dit adres wordt in gebruikersdialogen weergegeven als
*/

// 10AAAAAA 0AAA0AA1 DDDDDDDD (drei Byte Format) EXTENDED ACCESSOY

    
StateFunction( awaitPacketSent )
{
    if( bitRead(TIMSK,ENABLE_BIT) ) return 0 ;             // if previous packet is being transmitted, return false ;
    else
    {
        beginTime = micros() ; 
        toSend = toFill  ;                              // set buffer transmitt index correctly
        TCNT  = 0 ;                                    // initialize timer 2
        OCRA = DCC_ONE_BIT ;
        TIMSK |= (1 << ENABLE_BIT);                        // and fire up the ISR..
        return 1 ;
    }
}



// STATE MACHINE
void DCCsignals()
{
    STATE_MACHINE_BEGIN( sm )
    {
        State( selectNewPacketType ) {
            sm.nextState( assemblePacket, 0 ) ; }

        State( assemblePacket ) {
            sm.nextState( awaitPacketSent, 0 ) ;  }
        
        State( awaitPacketSent ) {
            sm.nextState( selectNewPacketType, 0 ) ;  }
    } 
    STATE_MACHINE_END( sm )
}

/***** INTERRUPT SERVICE ROUTINE ***** /        
ISR(TIMER_COMPA_vect)
{

    static uint8 state = preAmble ;
    static uint8 preAmbleCounter ;
    static uint8 *ptr ;
    static uint8 bitMask ;
    static uint8 ISR_state = 0 ; 

    //if( hb ) hb() ;

    if( dccISR ) dccISR() ; // call this to toggle IO

    ISR_state ^= 1 ;
    if(ISR_state) return ;

    switch( state )
    {
    case preAmble:
        OCRA = DCC_ONE_BIT ;
        if( oneBit )  oneBit() ;
        if( preAmbleCounter++ == maxPreAmble )
        {
            preAmbleCounter = 0 ;
            state = sendZero ;
            ptr = &buffer[toSend].data[0]-1 ; // load the pointer to the first byte
        }
        break ;

    case sendZero:
        OCRA = DCC_ZERO_BIT ; 

        if( zeroBit2 ) zeroBit2() ; // also shows spaces

        state = sendByte ; 
        ptr++ ;
        bitMask = 0x80 ;
        buffer[toSend].length -- ;
        break ;

    case sendByte:
        // Serial.println("zend byte");
        if( *ptr & bitMask ){ OCRA =  DCC_ONE_BIT ; }
        else                { OCRA = DCC_ZERO_BIT ; }
        bitMask >>= 1 ;

        if( OCRA ==  DCC_ONE_BIT ) if(  oneBit )  oneBit() ;
        if( OCRA == DCC_ZERO_BIT ) if( zeroBit ) zeroBit() ;
        
        
        if( bitMask == 0 ) // NOTE may need to be a 0
        {
            state = sendZero ;
            if( buffer[toSend].length == 0 ) // NOTE, MAY BE BECOME POST FIX
            {
                state = sendOne ;
            }
        }
        break ;

    case sendOne:                   // send final 1 bit to flag end of dcc packet
        if( endBit ) endBit() ;
        OCRA = DCC_ONE_BIT ;
        state = theEnd ;
        break ;

    case theEnd:
        bitClear(TIMSK,ENABLE_BIT) ; // kill ISR and flag main routine that packet is transmitted.
        state = preAmble ;
        Serial.println(micros() - beginTime ) ;
        break ;
    }
} // toggle pin 8 and 9 for direction*/

/*

*/

void initDCC()                     // initialize the timer
{
                // CSxx = pre-scaler
/*
CS12     CS11     CS10     MAY DIFFER FOR TIMER 0 AND 2
  0       0       0     No Clock Timer STOP
  0       0       1     CLK i/o /1     No Prescaling
  0       1       0     CLK i/o /8    (From Prescaler)
  0       1       1     CLK i/o /64   (From Prescaler)
  1       0       0     CLK i/o /256  (From Prescaler)
  1       0       1     CLK i/o /1024 (From Prescaler)

Timer overflow ISR. Called when incrementing timer reaches max value
TOIEx  in TIMSKx registers enables this ISR

OUTPUT compare Register OCRxA/B
OCIExy in TIMSKx enables thiS ISR (TIMERx_COMPy_vect) 
 CTC timer interrupts are triggered when the counter reaches a specified value, stored in the compare match register

Timer Input Capture: set by ICIEx in TMSKx register
UNSURE how this works

    
*/
// TCCR_A   Timer/Counter Control Register A
// COM2A1 COM2A0 COM2B1 COM2B0 – – WGM_1 WGM20
// COM are used for PWM

    cli() ; 

    TCCR_A = 0 ; // set entire TCCR_A register to 0
    TCCR_B = 0 ;

    TCCR_A |= (1 << WGM_1 ) ;

#ifdef TIMER2
    TCCR_B |= (0 << CS_2) | (1 << CS_1) | (0 << CS_0); // set prescaler to 8
#endif
   
    TCNT  = 0 ;
    //OCRA = 115;
    //bitSet( TIMSK, ENABLE_BIT ) ;   // fire up the ISR     

    sei() ;
}
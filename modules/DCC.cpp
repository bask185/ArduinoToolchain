/*
DCC CENTRAL by Bas Knippels

This software is a very basic DCC central. It can:
*	control DCC trains up to address 99
*	control 28 speed steps in both directions
*   control functions F0 through F8

What it cannot do:
*	read or control or do anything with CV's
*	it has no loconet or P51 or expressNet or any other known protocols embedded.

USAGE:
You can call these functions to controll the trains

  setSpeed( address, value ); 		enter 0 <> 56 for speeds -28 <> + 28
  setHeadlight( address, value ); 	1 or 0
  setFunctions( address, value ); 	1 - 99 are available
  addLoco( address, decoderType );
  deleteLoco( address );

decoderTypes {
	MM2,
	DCC14,
	DCC28,
	SELECTRIX,
	EMPTY_SLOT = 255};

MM2 and selectrix are not implemented at this point.
The decoder types are stored within the EEPROM memory

THE WORKS:
The central has 2 array buffers used to transmitt data. For the sake of efficiency 1 buffer is filled while the other buffer is clocked out to the tracks
New instructions are put in a FIFO type buffer. This allows for more than 1 parallel process to control trains. It is possible to let this buffer
overflow, so make sure you don't continously call the controll functions. Once is enough per change.

If you want to read out the current functions of any train. You can call the getTrain function of type Train like:
	Train currentTrain = getTrain( 40 );

This struct contains these variables for you to read out.
uint8_t	speed
uint8_t	headlight
uint8_t	functions
uint8_t	decoderType

Also important note: 
Setting a function is done by sending one whole '1' in a byte. bit 0 corresponds with F1 bit 7 corresponds with F8
Example, setting F4:
	setFunctions( 30, 0b00001000);

To clear a fuction you have to send the inverse instead.
Example clearing F6 is done by
	setFunctions( 20, ~0b00100000 ); // or
	setFunctions( 20,  0b11011111 ); // same thing, different writing method

If you want to toggle a function you first have to read the current function state, and set or clear your bit accordingly

This is done like this because if you set or clear a function it is not needed to know the current state of all 8 functions.
You can also controll 1 function per time with the setFunctions() method. This method should prevent people from accidently
controlling other unintended functions.

*/

// HEADER FILES
#include "DCC.h"
#include <EEPROM.h>

#include "src/basics/io.h" 

// CONSTANTS

const int DCC_ZERO_BIT =  1855; // 116us
const int DCC_ONE_BIT =  927;   // 58us
const int MAXIMUM_CURRENT =  169; // <- true for 0.33R shunt resistor for track current for 2.5 ampere.
const int CLEAR_SPEED =  0b11100000;
const int bufferSize  = 250;
const int repeatAmount =  20;

enum states {
	selectNewPacketType ,
	assemblePacket ,
	awaitPacketSent ,
};

enum packetTypes {
	speedPacket = 1 ,
	functionPacket1 ,
	functionPacket2 ,
	functionPacket3 ,
};

uint8_t debug = 0;
void startDebug() 			 { if(debug) Serial.println("%%start log"); }	// sending %% lets the central know not to relay bytes to handcontroller untill 0x80 is received
#define printDebug(x)  		   if(debug) Serial.print(x)
#define printDebugln(x)	       if(debug) {Serial.println(x); }
#define printBinln(x)	       if(debug) {Serial.println(x, BIN); }
void endDebug()				 { if(debug) Serial.write(0x80); }

// VARIABLES
Train train[ nTrains ] ; // 100 train objects are created here

struct {
	uint8_t In;							// In and Out counter are used to for FIFO buffer
	uint8_t Out;
	uint8_t newCommands[bufferSize];	// FIFO buffer used to store new commands
	uint8_t toFill : 1;					// one array may be filled...
	uint8_t toSend : 1;					// ... while the other one is clocked out to the tracks
	uint8_t array[2][8];
	uint8_t byteCount;					// these variable keeps track of ammount of bytes in FIFO buffer
} buffer;								// in the event of too many instructions the program will give an error message

uint8_t *ptr;
uint8_t packetType = speedPacket, sendPacket_state = 0, Bit;	// for DCC packets
uint8_t currentAddres=0, selectedAddress=0;	// must be int to prevent overflows during makeAddres()
uint8_t newInstructionFlag, state = assemblePacket;

// CONTROL FUNCTIONS
enum instructions {
	speedInstruction,
	setF1F4,
	clrF1F4,
	setF5F8,
	clrF5F8,
	headlightInstruction,
};

void setDecoderType( uint8_t _address, uint8_t type ) {
	train[ _address ].decoderType = type;
}

uint8_t checkAddress( uint8_t _address ) {
	for( int j = 0 ; j < nTrains; j ++ ) {
		if( train[j].decoderType <= 2 ) return true;
	}
	return false;
}

void dumpData()
{
}

void scoopInBuffer(uint8_t _address, uint8_t _instruction ) {
	if( !checkAddress ) return; // if address does not exist, disregard the instruction

	buffer.newCommands[ buffer.In ++ ] = _address;
	buffer.newCommands[ buffer.In ++ ] = _instruction;

	buffer.byteCount += 2;
	//Serial.print( "buffer in < "); Serial.println(buffer.byteCount);
	if( buffer.byteCount >= bufferSize ) {
		Serial.println("INSTRUCTION BUFFER HAS OVERFLOWED, YOU MORRON!!! (don't be offened, it's a joke)");
		Serial.println("CATASTROPHIC SYSTEM FAILURE, PROGRAM HAS TERMINATED"); 
		Serial.println("you sent too many bytes into the FIFO buffer, correct this");
		digitalWrite( power_on, LOW );
		//cli();
		while ( 1 ) {
			delay( 100 ); 	// blink led fast when this is the case
			digitalWrite( 13, HIGH );
			delay( 100 ); 
			digitalWrite( 13, LOW );
		}
	}
}

void scoopOutBuffer(uint8_t *_address, uint8_t *_instruction) {
	uint8_t _data;

	*_address	   = buffer.newCommands[ buffer.Out ++ ];
	*_instruction  = buffer.newCommands[ buffer.Out ++ ];

	buffer.byteCount -= 2;

	// static unsigned long prevTime;
	// unsigned long currentTime = millis();
	//Serial.print( "buffer out > "); Serial.println(buffer.byteCount);
	//Serial.print("time "); Serial.println(currentTime - prevTime);
	//prevTime = currentTime;

	uint8_t _state = *_instruction;
	switch( _state ) {
		case speedInstruction:		packetType = speedPacket; 	  break;
		case setF1F4:				packetType = functionPacket1; break;
		case clrF1F4:				packetType = functionPacket1; break;
		case setF5F8:				packetType = functionPacket2; break;
		case clrF5F8:				packetType = functionPacket2; break;
		case headlightInstruction:	packetType = functionPacket1; break;
	}

	//uint8_t data = 
	//Serial.print("train ");Serial.println( *_address );Serial.print("speed = ");Serial.println( train[ *_address ].speed  ); // new instructions are printed after they are processed
	//Serial.print("functions ");Serial.println( (uint8_t)train[ *_address ].functions );
	//Serial.print("headlight ");Serial.println( (uint8_t)train[ *_address ].headLight );

	newInstructionFlag = true;
}

// CONTROL FUNCTIONS TO CONTROLL DCC CENTRAL WITH
Train getTrain( uint8_t _address ) {
	return train[ _address ] ;
}

void setSpeed( uint8_t _address, uint8_t _speed) {
	train[ _address ].speed = _speed;
	scoopInBuffer( _address, speedInstruction ) ;
}

void Estop() {
	
}

void stop() {
}

void setHeadlight( uint8_t _address, uint8_t on_off) {
	train[ _address ].headLight = on_off;
	scoopInBuffer(_address ,headlightInstruction );
}

void setFunctions( uint8_t _address, uint8_t _functions) {
	uint8_t ON, instruction, bitCounter = 0;



	for( uint8_t j = 0 ; j < 8 ; j ++ ) {
		if( _functions & ( 1 << j ) ) bitCounter++;
		if( bitCounter > 1) break;
	}

	if( bitCounter > 1 ) { // more than 1 bit high? -> clear function
		if( _functions & 0x0F ) { instruction = clrF1F4; Serial.println(" clrF1F4"); }
		else					{ instruction = clrF5F8; Serial.println(" clrF5F8"); }
	}
	else {					// just 1 bit hifg, set function
		if( _functions & 0x0F )	{ instruction = setF1F4; Serial.println("setF1F4"); }
		else					{ instruction = setF5F8; Serial.println("setF5F8"); }
	}
	Serial.println(_functions, BIN);

	switch( instruction ) {
		case setF1F4:	train[ _address ].functions |=  _functions; break;
		case clrF1F4:	train[ _address ].functions &=  _functions; break;
		case setF5F8:	train[ _address ].functions |=  _functions; break;
		case clrF5F8:	train[ _address ].functions &=  _functions; break;
	}
	scoopInBuffer( _address, instruction );
}

void turnPower(uint8_t _power ) {
	if( _power ) {
		digitalWrite( power_on, HIGH );
	}
	else {
		digitalWrite( power_on, LOW );
	}
}

/******** STATE FUNCTIONS ********/
#define stateFunction(x) static unsigned char x##F()


stateFunction( selectNewPacketType ) {								// selects new packet type as well as new address
	static uint8_t newInstructionCouter = 0, previousAddress;

	if( newInstructionFlag ) { 										// new instructions must be repeated atleast this many times before anything else may be printed
		if( ++newInstructionCouter == repeatAmount ) {
			newInstructionCouter = 0;
			newInstructionFlag = false;
		}
		else {
			return 1; 												// keep using current address and packet type
		}
	} 

	if( buffer.In != buffer.Out ) {									// if in != out there is another new instruction in the buffer to be sent
		scoopOutBuffer( &currentAddres, &packetType ); 				// this sets new address as well as packet type
		newInstructionFlag = 1;
	}
	else {			
		pickNewAddress:												// normal cycling of instructions
		for( uint8_t i = currentAddres + 1 ; i <= nTrains - 1 ; i++ ){
			if( train[i].decoderType <= DCC28 ) {					// check which loco is next in line
				currentAddres = i ;
				//Serial.print("cur addres = ");Serial.println(currentAddres);
				break;
			}
		}
		if( currentAddres == previousAddress ) { 					// if these addresses are still the same it means we have reached the last of active loco's
																	// then we select the following packet types
			//Serial.print("cur packetType = ");	
			switch( packetType ) {
				case speedPacket: 		packetType = functionPacket1;/*Serial.println(packetType);*/ break;
				case functionPacket1:	packetType = functionPacket2;/*Serial.println(packetType);*/ break;
				case functionPacket2:	packetType = speedPacket; 	 /*Serial.println(packetType);*/ break;
			}
			currentAddres = 0;
			goto pickNewAddress;
		}
		previousAddress = currentAddres ; 
		return 1;
	}
	return 1;
}


stateFunction(assemblePacket) {
	startDebug();

	struct {
		unsigned char addres;
		unsigned char speed;
		unsigned char functions1;
		unsigned char functions2;
		unsigned char checksum; 
	} Packet;
	
	static unsigned char prevDir; // used to remember what the train's direction was in case the new speed becomes 0. This is needed so the light won't toggle if a train stops
		
	buffer.toFill ^= 1; // toggle to the other buffer to fill

/******	ADDRES	*******/ 
	Packet.addres = currentAddres;

	//if( packetType == 1) {		// merely code for debugging purposes
	printDebug("current addres "); printDebugln(currentAddres);
	printDebug("packet type    "); //printDebugln(packetType);
	switch( packetType ) {
		case 1: printDebugln("speed"); break;
		case 2: printDebugln("function pack 1"); break;
		case 3: printDebugln("function pack 2"); break;
	}
		//printDebugln("1 = speed, 2 = function pack 1, 3 = function pack 2");
	//}
	printDebug("speed    : "); printDebugln( train[currentAddres].speed );
	printDebug("functions: "); printDebugln( train[currentAddres].functions );
	printDebug("headlight: "); printDebugln( train[currentAddres].headLight );
	

	switch(packetType){																		// there are 3 types of packets, speed, F1-F4 + HL and F5-F8
		signed char speed_tmp;																// make local variable for speed to calculate with
	
	/******	SPEED	*******/ 
	case speedPacket:
		speed_tmp = train[currentAddres].speed - 28;
		Packet.speed = prevDir;
		if( speed_tmp < 0 ) {														// reverse operation
			speed_tmp = -speed_tmp;
			Packet.speed = REVERSE; }
		else if( speed_tmp > 0 ) {																				// forward operation
			Packet.speed = FORWARD;	}	
		prevDir = Packet.speed;												// store speed in 'speed_tmp' to calculate with

		if(speed_tmp & ESTOP_MASK) {
			Packet.speed &= CLEAR_SPEED;													// sets the speed at E-stop without modifying the direcion bits
			Packet.speed |= 1; } 
		else {

			if(train[currentAddres].decoderType == DCC14) speed_tmp /= 2;
			if(speed_tmp == 0) {															
				//Packet.speed &= CLEAR_SPEED; 
				Packet.speed &= 0b11100000;}// stop												// sets the speed at 0 without modifying the direction bits
			else {						
				if( train[currentAddres].decoderType == DCC28 ) {									// equal speed steps for DCC28 need the 5th bit set
					Packet.speed |= ( ( speed_tmp + 3 ) / 2 );
					if(speed_tmp % 2 == 0 ) Packet.speed |= ( 1 << 4 ); }						// set extra speed bit for DCC 28 decoders
				else if( train[currentAddres].decoderType == DCC14 ) {
					Packet.speed |= (speed_tmp + 1);
					if(train[currentAddres].headLight) Packet.speed |= ( 1 << 4 ); } } }		// set headlight bit for DCC 14 decoders NOTE might be unneeded as we might be able to this in the first function unsigned char
		
		buffer.array[ buffer.toFill ] [ 4 ] = Packet.speed; 
		break;
	
	/******	FUNCTIONS F1 - F4	*******/	
	case functionPacket1:
		Packet.functions1 = FUNCTIONS1;															
		Packet.functions1 |= (train[currentAddres].functions & 0x0F);							// F1 - F4
		if(train[currentAddres].headLight && train[currentAddres].decoderType == DCC28) {
			Packet.functions1 |= (1<<4); 
		}					 																	// turns light on for DCC 28 decoders, the if statement might be useless as it may do no harm to do this for dcc 14 decoders as well
		buffer.array[ buffer.toFill ] [ 4 ] = Packet.functions1;
		break; 
			
	/******	FUNCTIONS F5 - F8	*******/ 
	case functionPacket2: 
		Packet.functions2 = FUNCTIONS2;															
		Packet.functions2 |= (train[currentAddres].functions >> 4);								// F5 - F8
		buffer.array[ buffer.toFill ] [ 4] = Packet.functions2;
		break; 
	}

	/******	FUNCTIONS F9 - F12	*******/
	/*case functionPacket3:																	// NOTE future usage
		Packet.functions3 = FUNCTIONS2;															
		Packet.functions3 |= (train[currentAddres].functions2 >> 4);							// F9 - F12
		buffer.array[3] = Packet.functions3;
		break; }*/

	/***** CHECKSUM *******/	
	Packet.checksum = Packet.addres ^ buffer.array[ buffer.toFill ] [ 4 ];									
	
	/***** SHIFT PACKETS FOR  BUFFER ARRAY *******/ 
	buffer.array[ buffer.toFill ] [ 0 ] = 0x01;																// pre amble
	buffer.array[ buffer.toFill ] [ 1 ] = 0xFF;
	buffer.array[ buffer.toFill ] [ 2 ] = 0xFC;	// pre-amble + 2 0 bits						 				// pre amble, 0 bit, 1st 0 bit of addres unsigned char
	buffer.array[ buffer.toFill ] [ 3 ] = Packet.addres << 1;												// addres unsigned char, 0 bit
//	buffer.array[ buffer.toFill ] [ 4 ] is filled above
	buffer.array[ buffer.toFill ] [ 5 ] = Packet.checksum >> 1;												// 0 bit, 7 bits of checksum
	buffer.array[ buffer.toFill ] [ 6 ] = Packet.checksum << 7 | 1 << 6; 

//		11111111111111 0 11111111 0 00000000 0 11111111 1	<- IDLE PACKET
//
//		 0			 1			2			3			 4			5	
//	11111111	111111_0_1	 1111111_0	 00000000	 0_1111111 	11000000
//		0xFF	 0xFD		 0xFE		0x00			0x7F	0xC0
	
	// else {																											// if current addres = 0,OBSOLETE IDLE packets are no longer send
	// 	printDebugln("idle packet");
	// 	buffer.array[ buffer.toFill ] [ 0 ] = 0x01;																		// we overwrite the buffers to 
	// 	buffer.array[ buffer.toFill ] [ 1 ] = 0xFF;																		// we overwrite the buffers to 
	// 	buffer.array[ buffer.toFill ] [ 2 ] = 0xFD;																		// assemble an IDLE packet
	// 	buffer.array[ buffer.toFill ] [ 3 ] = 0xFE;																		
	// 	buffer.array[ buffer.toFill ] [ 4 ] = 0x00;																		
	// 	buffer.array[ buffer.toFill ] [ 5 ] = 0x7F;
	// 	buffer.array[ buffer.toFill ] [ 6 ] = 0xC0; 
	// }


	// printDebug("address  "); printDebugln( Packet.addres );
	// printDebug("speed    "); printDebugln( buffer.array[ buffer.toSend ][3] & 0b11111 );
	// printDebug("checksum "); printDebugln( Packet.checksum) ;

	endDebug();

	return true;
}
	
stateFunction(awaitPacketSent) {
	if( bitRead(TIMSK1,OCIE1B) ) return 0;										// if previous transmission is still bussy, return false;
	else {																		
		buffer.toSend = buffer.toFill ;											// select other buffer
		ptr = &buffer.array[ buffer.toSend ] [ 0 ];								// let the pointer point to the correct ellement

		bitSet(TIMSK1,OCIE1B);	// fire up the ISR
		return 1;
	}
}



// STATE MACHINE
byte runOnce = true;
static void nextState(byte newState) {
	runOnce = 1;
	state = newState;
}
#define State(x) break; case x: /*if(runOnce){Serial.println(#x);runOnce=0;}*/if(x ## F())
extern void DCCsignals(void) {
	switch(state){
		default: state = selectNewPacketType; Serial.println("UNKNOWN STATE ENTERED, CRITICAL BUG"); // Should not ever be sent
		
		State( selectNewPacketType )	nextState( assemblePacket );

		State( assemblePacket )			nextState( awaitPacketSent ); 
		
		State( awaitPacketSent )		nextState( selectNewPacketType ); 
		
		break; 
	} 
}


/***** INTERRUPT SERVICE ROUTINE *****/		
ISR(TIMER1_COMPB_vect) {
	static unsigned char bitMask = 0x80, ISR_state = 0;;

	PORTD ^= 0b00011000; 													// pin 3 and 4 needs to be toggled
	
	ISR_state ^= 1;
	if(ISR_state) {															// pick a bit and set duration time accordingly		

		if( *ptr & bitMask ){ OCR1A = DCC_ONE_BIT;	}						// '1' 58us
		else			   	{ OCR1A = DCC_ZERO_BIT;	}						// '0' 116us

		if( bitMask == 0x40 && ptr == &buffer.array[ buffer.toSend ][6] ) {	// last bit?
			bitClear(TIMSK1,OCIE1B);										// kill ISR
		} 
		else {																// if not last bit, shift bit mask, and increment pointer if needed
			bitMask >>= 1;
			if(!bitMask) {
				bitMask = 0x80;
				ptr++;
			} 
		} 
	}

	else {
		ISR_state = 0; 
	} 
} // toggle pin 8 and 9 for direction*/



void initDCC() {					 // initialize the timer 
	cli(); 
	bitSet(TCCR1A,WGM10);		 
	bitSet(TCCR1A,WGM11);
	bitSet(TCCR1B,WGM12);
	bitSet(TCCR1B,WGM13);

	bitSet(TCCR1A,COM1B1);		
	bitSet(TCCR1A,COM1B0);

	bitClear(TCCR1B,CS12);		// set prescale on 1
	bitClear(TCCR1B,CS11);
	bitSet(TCCR1B,CS10);

	OCR1A = DCC_ONE_BIT;
	sei();

	for( int i = 1 ; i < nTrains ; i ++ ) {
		//uint8_t type = EEPROM.read( i ); DISABLED FOR THE TIME BEING
		//setDecoderType( i, type );
		train[i].decoderType = DCC28;
		train[i].speed = 28; // speed 0
		train[i].functions = 0x00; // turn off all functions
		train[i].headLight = 1; // yes we turn on headlights 
	}
	// for( int i = 1 ; i < nTrains ; i += 3 ) {
	// 	uint8_t type = EEPROM.read( i );
	// 	setDecoderType( i, type );
	// 	train[i].decoderType = DCC28;
	// }
	sei();
}// on/off time?


// void print_binary(byte *ptr) { // OBSOLETE FUNCTION was used in the past to verify packet content
// 	byte bitMask = 0x80, counter = 0;
// 	while(1) {
// 		if(*ptr & bitMask)	Serial.write('1');
// 		else 				Serial.write('0');

// 		bitMask >>= 1;
// 		if(bitMask == 0) {
// 			bitMask = 0x80;
// 			ptr++ ;
// 		}
// 		counter++;
// 		if(counter == 14 || counter == 15 || counter == 23 || counter == 24 || counter == 32 || counter == 33 || counter == 41) Serial.write(' ');
// 		if(counter == 27) Serial.write('.');
// 		if(counter == 42) {
// 			Serial.println();
// 			break;
// 		}
// 	}
// }
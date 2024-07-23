// MACROS
#ifndef MACROS
#define MACROS

// #define DEBUG

#define REPEAT_US(x)    { \
                            static uint32_t previousTime ;\
                            uint32_t currentTime = micros() ;\
                            if( currentTime  - previousTime >= x ) {\
                                previousTime = currentTime ;
                                // code to be repeated goes between these 2 macros
#define REPEAT_MS(x)    { \
                            static uint32_t previousTime ;\
                            uint32_t currentTime = millis() ;\
                            if( currentTime  - previousTime >= x ) {\
                                previousTime = currentTime ;
                                // code to be repeated goes between these 2 macros
#define END_REPEAT          } \
                        }


#define toggleState static uin8t state ;  state ^= 1 ;                          // creates a static local variable named 'state' and toggle it
#define iterate(var, x)  if( ++ var == x ) var = 0 ;  // creates a static local variable named 'index' and increment it to given limit before resetting
#define ARRAY_SIZE( arr) (sizeof(arr)/sizeof(0[arr]))

#define startupDelay( interval ) waitStartDelay : if( millis() <= interval ) goto waitStartDelay ;

#define printNumber( txt, x )   Serial.print( txt ) ; Serial.print( x ) ;
#define printNumber_( txt, x )  Serial.print( txt ) ; Serial.print( x ) ;Serial.write(' ');
#define printNumberln( txt, x ) Serial.print( txt ) ; Serial.println( x ) ;

// #define lowByte(x)          ((x) & 0x00FF )
// #define highByte(x)         (((X) & 0xFF00 ) > 8)

#define int8                int8_t
#define uint8               uint8_t
#define int16               int16_t
#define uint16              uint16_t
#define int32               int32_t
#define uint32              uint32_t

#define SET(x,y)            ((x) |=  (1 << (y)))
#define CLR(x,y)            ((x) &= ~(1 << (y)))
#define TGL(x,y)            ((x) ^=  (1 << (y)))
#define READ(x,y)           (((x) >> (y)) & 1)


#define STRING char*
#define STR2(s)             #s
#define STR(s)              STR2(s)


#endif
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

#define printNumber( txt, x )   Serial.print( txt ) ; Serial.print( x ) ;
#define printNumber_( txt, x )  Serial.print( txt ) ; Serial.print( x ) ;Serial.write(' ');
#define printNumberln( txt, x ) Serial.print( txt ) ; Serial.println( x ) ;

#define lowByte(x)          ((x) & 0x00FF )
#define highByte(x)         (((X) & 0xFF00 ) > 8)

#define INT8                int8_t
#define UINT8               uint8_t
#define INT16               int16_t
#define UINT16              uint6_t
#define INT32               int32_t
#define UINT32              uint32_t

#define SET(x,y)            ((x) |=  (1 << (y)))
#define CLR(x,y)            ((x) &= ~(1 << (y)))
#define TGL(x,y)            ((x) ^=  (1 << (y)))
#define READ(x,y)           (((x) >> (y)) & 1)


#define STRING char*
#define STR2(s)             #s
#define STR(s)              STR2(s)


#endif
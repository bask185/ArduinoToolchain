// MACROS
#ifndef MACROS
#define MACROS

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


#endif
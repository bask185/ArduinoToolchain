/* NOTES TO MYSELF
    code is shitty at the moment, it will most likely work, but it may also detect holes between cars

    I think I need to calculate an average with like 5 samples and an average with like 25 sample
    If a sudden drop occurs, the 'small' average will change faster than the 'large' average.
    With this you might 'skip' the distance between cars while you remain background noise

    The background average may also be taken with a larger interval time. The primary goal
    is to deal with background light 'noise' and it would be ok if a full average would take like
    a minute or so.

    should also not forget to force the average to the current ammount of light when I call begin


*/
#include "LDR.h"







LDR::LDR( uint8_t _pin ) { 
    pin = _pin ; }

void LDR::begin() {
    pinMode( _pin, INPUT ) ;
}

void update() {
    uint32_t currentTime = millis();

    if( currentTime > ( previousTime + sampleInterval ) ) {
        previousTime = currentTime ;

        uint16_t averageSample = 0;
        uint16_t currentSample = 0;

        currentSample = analogRead( pin ) ; // take sample

        samples[ sampleCounter ] = currentSample ;

        if( debug ) {
            Serial.print("currentSample: ") ;
            Serial.println( currentSample ) ;
        }

         sampleCounter ++ ;
         if( sampleCounter > nSamples ) sampleCounter = 0 ;

        for( uint8_t j = 0 ; j < nSamples ; j ++ ) {
            averageSample += samples[ j ] ;                     // add all samples 
        }

        averageSample /= nSamples ;                             // calculate average sample
        averageSample = constrain( averageSample, sensitivity + 1, 1023 - sensitivity - 1 ) ;  // allows some room for below calculations

        if( debug ) {
            Serial.print("average: ") ;
            Serial.println( samples[ sampleCounter ] ) ;
        }

        if( currentSample < ( averageSample - sensitivity ) 
        ||  currentSample > ( averageSample + sensitivity ) ) { // if a very sudden change occurs

            for( uint8_t j = 0 ; j < nSamples ; j ++ ) {
                samples[ j ] = currentSample ;                 // set new average sample
            }
            
            state = true ;                                     // and set flag fot the state change
        }
    }
}

uint8_t LDR::getState() {
    if( state ) {
        state = 0 ;
        return 1 ;
    }

    return 0 ;
}


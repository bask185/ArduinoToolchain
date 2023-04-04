#include <Arduino.h>
#include "keypad.h"
#include "lcd.h"
#include "macros.h"
#include <EEPROM.h>
#include "debounceClass.h"

Keypad::Keypad(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7, uint8_t pin8) 
{
    keyPin[0] = pin1;
    keyPin[1] = pin2;
    keyPin[2] = pin3;
    keyPin[3] = pin4;
    keyPin[4] = pin5;
    keyPin[5] = pin6;
    keyPin[6] = pin7;
    keyPin[7] = pin8; 
}

Debounce keyDebounced[] =
{
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1),
    Debounce(-1)
} ;
		

void Keypad::init(void) 
{
    for(uint8_t i = 0; i < nColumn; i++) 
    {  
        pinMode(keyPin[i], OUTPUT); 
    }
        
    for(uint8_t i = 0; i < nRow; i++) 
    {  
        pinMode(keyPin[i+nColumn], INPUT_PULLUP); 
    }

    // for( int i = 0 ; i < 0x180 ; i ++ )
    // {
    //     uint16_t b = EEPROM.read(i) << 8 | EEPROM.read(i+1) ;
    //     setCur(0,0);
    //     printNumberU(b,4);
    //     setCur(0,1);
    //     printNumberU(i,4);
    //     delay(1000);
    // }
}

/**
 * @brief updates and debounces the keypad
 *
 * @param N/A
 *
 * @return N/A
 */
void Keypad::update( ) 
{
    for( uint8_t column = 0 ; column < nColumn ; column ++ )
    {
        for(uint8_t i = 0 ; i < nColumn ; i ++ ){
             digitalWrite(keyPin[i],HIGH);    // set all 4 column pins HIGH
        }
        digitalWrite(keyPin[column], LOW);                                        // set 1 column pin LOW

        for( uint8_t row = 0 ; row < nRow ; row  ++ ) 
        {
            
            bool state = digitalRead( keyPin[row+nColumn] );                     // for every debounce inputs

            uint8_t currentKey = ((row) + (column*nColumn)) ;
            //if( state ==  LOW ) print("L") ;
            //if( state == HIGH ) print("H") ;
            //printNumberAt(0,1,column);
            //printNumberAt(4,1,row);
            //printNumberAt(8,1,currentKey);
            keyDebounced[currentKey].debounce( state ) ;
        }
    }
}

/**
 * @brief reads keypad for button presses
 *
 * @param address of variable to store the key in.
 *
 * @return 0: no key pressed
 *         1: a key is pressed
 *         2: a key is pressed longer than 1.5s
 */
uint8_t Keypad::readKey( char * retKey ) 
{
    const int pressInterval = 750 ;
    static uint32_t previousTime ;
    static bool discardRising = false ;
    uint32_t currentTime = millis() ;
    
    for( uint8_t column = 0 ; column < nColumn ; column ++ )
    {
        for( uint8_t row = 0 ; row < nRow ; row ++ )
        {
            uint8_t currentKey = ((row) + (column*nColumn)) ;
            uint8_t state = keyDebounced[currentKey].getState( ) ;

            if( state == RISING )
            {
                if( discardRising == true )
                {
                    discardRising = false ;
                }
                else
                {
                    *retKey = keys [ row ] [ column ] ;
                    return 1 ;
                }
            }
            if( state == FALLING )
            {   
                previousTime = currentTime ;
            }
            if( state == LOW )
            {
                if( (currentTime - previousTime >= pressInterval) && discardRising == false )
                {
                    previousTime = currentTime ;
                    *retKey = keys [ row ] [ column ] ;
                    discardRising = true ;
                    return 2 ;
                }
            }

        }
    }
    return 0; 
}


Keypad keypad(12,11,10,9, 8,7,6,5); // GOTTA FIX THIS WITH REALITY ONE DAY

/**** wrapper functions ***/
void initKeypad()
{
    keypad.init() ;
}


/**
 * @brief wrapper function to read from keypad
 *
 * @param address of variable to store the key in.
 *
 * @return 0: no key pressed
 *         1: a key is pressed
 *         2: a key is pressed longer than 1.5s
 */
uint8_t getKey(char *key )
{
    return keypad.readKey( key ) ;
}

void debounceKeypad()
{
    REPEAT_MS(20) ;
    keypad.update() ;
    END_REPEAT
}
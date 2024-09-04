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

#define nColumn 4
#define nRow 4

class Keypad {
    public:
        Keypad(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
        void init(void);
        uint8_t readKey( char * );
        void update();
       

    private:
        uint8_t keyPin[8];
    
        const char keys[nColumn][nRow] = {
            {'1','2','3','A'},
            {'4','5','6','B'},
            {'7','8','9','C'},
            {'*','0','#','D'}
        };        
};

extern void initKeypad() ;
extern uint8_t getKey( char * ) ;
extern void debounceKeypad() ;
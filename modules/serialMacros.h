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

/* serial macros used to combine serial print/write functions in order to format a little bit and to type a lot less code */

// prints the description + a : followd by a space, and the actual value, can be an int or float whatever. Add Ln to create a new line
#define printVal(x,y)   Serial.print(#x);Serial.print(": ");Serial.print(y);Serial.print(" ");  // add space for next val
#define printValLn(x,y) Serial.print(#x);Serial.print(": ");Serial.print(y);Serial.println();   // add new line
newLines(x) for(int i = 0, i < x ; i++ )Serial.println();                                       // print x ammount of LF and RF
clearScreen() Serial.write(0x12);                                                               // sends instruction to whipe putty screen
homeCursor() Serial.write('\r');                                                                // sets the cursor to beginning without newline
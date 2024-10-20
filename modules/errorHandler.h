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

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <Arduino.h>

const int MAX_ERRORS = 10;

class ErrorHandler {
public:
    // Struct om een error te representeren
    struct Error {
        char message[32];
        bool (*resolver)();
    };

    ErrorHandler();

    // Voegt een error toe aan de queue
    bool pushError(const char* errorMessage, bool (*resolver)());

    // Verwerkt de huidige error als die bestaat
    void processErrors();

private:
    Error errorQueue[MAX_ERRORS];  // Array om errors te bufferen
    int errorCount;                // Aantal huidige errors in de queue
    int head;                      // Index van de eerste error (FIFO)
    int tail;                      // Index van de laatste error (FIFO)
    bool errorPrinted;             // Geeft aan of de huidige error al is geprint
};

#endif

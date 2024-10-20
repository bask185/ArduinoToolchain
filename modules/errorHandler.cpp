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

#include "errorHandler.h"

ErrorHandler::ErrorHandler() : errorCount(0), head(0), tail(0), errorPrinted(false) {}

bool ErrorHandler::pushError(const char* errorMessage, bool (*resolver)()) {
    if (errorCount >= MAX_ERRORS) {
        return false;  // Queue is vol
    }

    // Voeg nieuwe error toe aan de queue (op de tail positie)
    strncpy(errorQueue[tail].message, errorMessage, sizeof(errorQueue[tail].message) - 1);
    errorQueue[tail].message[31] = '\0';  // Zorg ervoor dat het eindigt met een null terminator
    errorQueue[tail].resolver = resolver;

    // Verhoog de tail en wrap-around als deze het einde van de array bereikt
    tail = (tail + 1) % MAX_ERRORS;
    errorCount++;
    
    return true;
}

void ErrorHandler::processErrors() {
    if (errorCount == 0) {
        return;  // Geen errors om te verwerken
    }

    // Print de huidige error slechts één keer
    if (!errorPrinted) {
        Serial.println(errorQueue[head].message);
        errorPrinted = true;
    }

    // Roep de resolver aan voor de huidige error
    if (errorQueue[head].resolver()) {
        // Als de error is opgelost, verwijder de huidige error (FIFO)
        head = (head + 1) % MAX_ERRORS;
        errorCount--;
        errorPrinted = false;  // Reset de print status voor de volgende error
    }
}

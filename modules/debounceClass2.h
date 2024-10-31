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
#ifndef DEBOUNCECLASS2_H
#define DEBOUNCECLASS2_H

#include <Arduino.h>
#include "macros.h"

// Author note. Comments are added by chatgpt

// Note: LOW, HIGH, FALLING, and RISING are already defined in Arduino.h

/// @brief Constant used to represent both rising and falling edges for setFlank().
const int BOTH = 1; 

/// @brief Constant used to represent a short press event.
const int SHORT = 1; 

/// @brief Constant used to represent a long press event.
const int LONG = 2; 

/**
 * @brief A class for debouncing digital signals and handling press events.
 * 
 * The Debouncer class is designed to debounce input signals (such as button presses)
 * and trigger callbacks for short and long press events. It can also track state changes
 * such as rising and falling edges.
 */
class Debouncer
{
public:
    /**
     * @brief Constructor for the Debouncer class.
     * 
     * Initializes default values for pins, debounce time, and flags.
     */
    Debouncer();

    /**
     * @brief Function pointer for handling short press events.
     * 
     * This callback is triggered when a short press (duration less than a specified limit) occurs.
     */
    void (*onShortPress)(Debouncer*) = nullptr;

    /**
     * @brief Function pointer for handling long press events.
     * 
     * This callback is triggered when a long press (duration greater than or equal to a specified limit) occurs.
     */
    void (*onLongPress)(Debouncer*) = nullptr;

    /**
     * @brief Function pointer for handling state changes.
     * 
     * This callback is triggered when the state of the input signal changes (e.g., from LOW to HIGH or vice versa).
     * 
     * @param Debouncer* Pointer to the Debouncer instance.
     * @param uint8 The new state of the signal.
     */
    void (*onStateChange)(Debouncer*, uint8) = nullptr;

    /**
     * @brief Sets the type of edge to trigger on.
     * 
     * Configures whether the debounce method should respond to rising edges, falling edges, or both.
     * 
     * @param _flank The edge type: RISING, FALLING, or BOTH.
     */
    void setFlank(uint8 _flank);

    /**
     * @brief Associates a pin number with the debouncer.
     * 
     * This method allows the Debouncer to read digital input directly from the specified pin.
     * 
     * @param _pin The pin number to read from.
     */
    void setPin(uint8 _pin);

    /**
     * @brief Sets the debounce time interval.
     * 
     * Configures the interval (in milliseconds) used to debounce the signal. A value of 0 disables internal timing.
     * 
     * @param _interval The debounce time interval in milliseconds.
     */
    void setDebounceTime(uint32 _interval);

    /**
     * @brief Tracks the duration of a press and triggers the appropriate callback.
     * 
     * This method measures the time between a falling and rising edge and triggers either
     * a short press or long press callback based on the specified time limit.
     * 
     * @param limit The time limit in milliseconds to differentiate between a short and long press.
     * @param useInternal If true, the method will call debounce() to process internal signals.
     * @return SHORT if the press is shorter than the limit, LONG if it's longer, or 0 if no press is detected.
     */
    uint8 pressTime(uint32 limit, uint8 useInternal = false);

    /**
     * @brief Checks if a rising edge has been detected.
     * 
     * This method checks if a rising edge (LOW to HIGH transition) has occurred and returns 1 if true.
     * 
     * @return 1 if a rising edge has occurred, 0 otherwise.
     */
    uint8 hasRisen();

    /**
     * @brief Checks if a falling edge has been detected.
     * 
     * This method checks if a falling edge (HIGH to LOW transition) has occurred and returns 1 if true.
     * 
     * @return 1 if a falling edge has occurred, 0 otherwise.
     */
    uint8 hasFallen();

    /**
     * @brief Debounces the input signal using digitalRead.
     * 
     * This method reads the input signal from the configured pin using digitalRead, applies debounce logic,
     * and returns the current debounced state.
     * 
     * @return The debounced state: LOW, HIGH, RISING, or FALLING.
     */
    uint8 debounce();

    /**
     * @brief Debounces the input signal with a provided sample.
     * 
     * This overloaded method processes an externally provided sample, applies debounce logic,
     * and returns the debounced state.
     * 
     * @param newSample The new input sample (HIGH or LOW).
     * @return The debounced state: LOW, HIGH, RISING, or FALLING.
     */
    uint8 debounce(uint8 newSample);

    // Public variables used for internal state tracking.
    uint16 state : 2;          ///< Current debounced state (LOW, HIGH, RISING, or FALLING).
    uint16 oldSample : 1;      ///< The previous sample used for debouncing.
    uint16 pin : 6;            ///< The pin number used for digital input.
    uint16 statePrev : 1;      ///< The previous state before the current state.
    uint16 newState : 2;       ///< The new debounced state after processing.
    uint16 flank : 2;          ///< The type of edge to trigger on (RISING, FALLING, or BOTH).
    uint16 Q : 1;              ///< Flag indicating if the current state matches the set flank.
    uint16 hasLongPressed : 1; ///< Flag indicating if a long press has been detected.
    uint16 hasRisenFlag : 1;   ///< Flag indicating if a rising edge has been detected.
    uint16 hasFallenFlag : 1;  ///< Flag indicating if a falling edge has been detected.

private:
    uint32 prevTime;           ///< The previous time (in milliseconds) used for debouncing.
    uint32 interval;           ///< The debounce time interval (in milliseconds).
    uint32 pressStartTime;     ///< The time when the button press started.
};

#endif // DEBOUNCECLASS2_H

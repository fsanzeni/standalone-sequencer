#pragma once
/**
 * @file analogIO.h
 * @brief Interface for handling analog inputs in the Arduino sequencer.
 *
 * This file defines the AnalogIo class which is responsible for:
 * - Initializing analog input handling with a reference to the sequencer.
 * - Polling analog inputs (regular and calibration modes).
 * - Displaying parameters based on analog inputs.
 * - Recording parameter changes.
 *
 * Additionally, several PROGMEM strings are defined to display names for audition
 * and CV mode settings.
 */

#include <Arduino.h>
#include <stdint.h>
#include "sequencer.h"

// PROGMEM definitions for audition modes
const char audition_0[] PROGMEM = "OFF"; // Normal behavior (audition off)
const char audition_1[] PROGMEM = "AUD"; // Audition mode (play note when changed)

// Array of pointers to audition mode strings stored in PROGMEM
const char *const audition_names[] PROGMEM = { audition_0, audition_1 };

// PROGMEM definitions for CV mode names
const char cvmode_0[] PROGMEM = " CV"; // CV auxiliary mode
const char cvmode_1[] PROGMEM = "LFO"; // Low Frequency Oscillator mode
const char cvmode_2[] PROGMEM = "INT"; // Internal mode
const char cvmode_3[] PROGMEM = "NOT"; // Note mode

// Array of pointers to CV mode strings stored in PROGMEM
const char *const cvmode_names[] PROGMEM = { cvmode_0, cvmode_1, cvmode_2, cvmode_3 };

class AnalogIo {
public:
    /**
     * @brief Initialize analog IO with a reference to the sequencer.
     * 
     * @param sequencer Reference to the Sequencer object.
     */
    void init(Sequencer& sequencer);

    /**
     * @brief Poll the analog inputs during normal operation.
     * 
     * @param shift_state Current state of the shift button.
     */
    void poll(bool shift_state);

    /**
     * @brief Poll the analog inputs during calibration mode.
     */
    void pollCalibration();

    /**
     * @brief Display the currently selected parameter.
     */
    void displaySelectedParam();

    /**
     * @brief Record the current parameter value.
     */
    void recordCurrentParam();

    /**
     * @brief Get the numeric value to be displayed.
     * 
     * @return int The numeric value.
     */
    int getDisplayNum();

    /**
     * @brief Get the alphanumeric display string.
     * 
     * @return char* Pointer to a 4-character display string.
     */
    char* getDisplayAlpha();

    /**
     * @brief Get the current calibration value from analog input.
     * 
     * @return int Calibration value.
     */
    int getCalibrationValue();

    /**
     * @brief Check if any analog parameter has changed.
     * 
     * @return true If parameter changed.
     * @return false Otherwise.
     */
    bool paramChanged();

    /**
     * @brief Check if the parameter should be displayed as alphanumeric.
     * 
     * @return true If parameter is alpha.
     * @return false If parameter is numeric.
     */
    bool paramIsAlpha();

    /**
     * @brief Set the record mode for analog inputs.
     * 
     * @param mode true to enable record mode, false to disable.
     */
    void setRecordMode(bool mode);

    /**
     * @brief Set the display mode based on analog input.
     * 
     * @param mode The mode value.
     */
    void setDisplayMode(int mode);

private:
    /**
     * @brief Read an analog input.
     * 
     * @param i The input index.
     * @param shift_state Current state of the shift button.
     * @param write_values Whether to write/process the read values.
     * @return true if the input was successfully read and processed.
     * @return false otherwise.
     */
    bool readInput(int i, bool shift_state, bool write_values);

    /**
     * @brief Set pitch based on the analog value.
     * 
     * @param analogValue The analog input value.
     */
    void setPitch(int analogValue);

    /**
     * @brief Set the octave based on the analog value.
     * 
     * @param analogValue The analog input value.
     */
    void setOctave(int analogValue);

    /**
     * @brief Set the note duration based on the analog value.
     * 
     * @param analogValue The analog input value.
     */
    void setDuration(long analogValue);

    /**
     * @brief Update the numeric value to be displayed.
     * 
     * @param displayNum Numeric value for display.
     */
    void setDisplayNum(int displayNum);

    /**
     * @brief Update the alphanumeric string to be displayed.
     * 
     * @param displayAlpha 4-character string to display.
     */
    void setDisplayAlpha(char displayAlpha[4]);

    /**
     * @brief Set the CV (control voltage) output based on analog value.
     * 
     * @param analogValue The analog input value.
     */
    void setCV(int analogValue);

    /**
     * @brief Set the CV mode based on analog input.
     * 
     * @param analogValue The analog input value.
     */
    void setCVMode(int analogValue);

    /**
     * @brief Set the audition mode based on the analog value.
     * 
     * @param analogValue The analog input value.
     */
    void setAudition(int analogValue);

    /**
     * @brief Display the pitch name on the display.
     * 
     * @param pitch The pitch value.
     * @param octave The octave value.
     */
    void displayPitchName(int pitch, int octave);

    /**
     * @brief Display the CV mode name on the display.
     * 
     * @param pitch The pitch value representing the CV mode.
     */
    void displayCvName(int pitch);

    // Variables to store display values.
    int display_param;  // Currently active parameter index for display
    int display_num;    // Numeric value to be displayed
};
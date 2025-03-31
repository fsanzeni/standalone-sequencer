/**
 * @file analogIO.cpp
 * @brief Implementation of analog input processing for the MEGA 2560 sequencer.
 *
 * This file implements the AnalogIo class functions. The AnalogIo class:
 * - Initializes analog pins and sets initial values.
 * - Polls analog inputs both during normal operation and in calibration mode.
 * - Processes analog input changes for pitch, octave, duration, CV and mode parameters.
 * - Updates the display based on the current parameter values.
 */

 #include <Arduino.h>
 #include <SPI.h>
 #include <stdint.h>
 #include <string.h>
 
 #include "pinout.h"
 #include "analogIO.h"
 #include "display.h"
 #include "sequencer.h"
 
 // Array of analog pins used by the system.
 const int analog_pins[4] = { 
	 ANALOG_PIN_1,
	 ANALOG_PIN_2,
	 ANALOG_PIN_3,
	 ANALOG_PIN_4
 };
 
 // Parameter indices for different analog functions.
 #define PITCH_PARAM    0
 #define OCTAVE_PARAM   1
 #define DURATION_PARAM 2
 #define CV_PARAM       3
 #define MODE_PARAM     4
 
 // Display mode settings.
 #define DISPLAY_MODE_NUMERIC   0
 #define DISPLAY_MODE_NOTE_NAME 127
 
 // Threshold values for analog value change detection.
 #define SHIFT_CHANGE_THRESHOLD   20
 #define DEFAULT_CHANGE_THRESHOLD 12
 #define LOW_CHANGE_THRESHOLD     5
 
 // Mapping for analog parameters.
 const int analog_params[4] = { PITCH_PARAM, OCTAVE_PARAM, DURATION_PARAM, CV_PARAM };
 
 // Global flags and variables for analog IO.
 bool editing = false;         			// Flag to allow writing of new analog values.
 bool audition = false;        			// Flag to enable audition (play note on change).
 int lastAnalogValues[4];    			// Store previous analog readings for change detection.
 int analogValues[4];        			// Latest analog readings.
 int analogMultiplexor = 0;  			// Used to cycle through analog inputs.
 int display_param = PITCH_PARAM;  		// Currently selected parameter for display.
 int display_num = 0;        			// Numeric value to be displayed.
 int display_mode = 0;       			// 0: note numbers; 127: note names (e.g. "C1", "B3").
 char display_alpha[4];      			// Alphanumeric display buffer (3 characters + terminator if needed).
 bool param_changed = false; 			// Flag indicating if the parameter has changed.
 int change_threshold = DEFAULT_CHANGE_THRESHOLD; // Dynamic threshold for change detection.
 bool recording = false;       			// Flag to indicate if recording is active.
 bool recorded_input_active = false; // Flag to indicate recent input activity.
 char modename[5];            			// Buffer to hold mode names from PROGMEM.
 
 Sequencer* sequencerVar = nullptr;  	// Pointer to the Sequencer instance
 
 /**
  * @brief Initialize analog IO and set initial values.
  *
  * Configures the analog input pins, stores the reference to the sequencer, 
  * and polls the analog inputs several times to stabilize initial readings.
  */
 void AnalogIo::init(Sequencer& sequencer) {
	 pinMode(ANALOG_PIN_1, INPUT);
	 pinMode(ANALOG_PIN_2, INPUT);
	 pinMode(ANALOG_PIN_3, INPUT);
	 pinMode(ANALOG_PIN_4, INPUT);
	 
	 sequencerVar = &sequencer;
	 
	 // Poll several times to initialize knob positions and avoid accidental edits.
	 poll(false);
	 poll(false);
	 poll(false);
	 poll(false);
	 
	 editing = true;
 }
 
 /**
  * @brief Poll the analog inputs during normal operation.
  *
  * Cycles through the available analog inputs and processes the value change
  * if the change exceeds a threshold.
  *
  * @param shift_state Current state of the shift button.
  */
 void AnalogIo::poll(bool shift_state) {
	 analogMultiplexor = (analogMultiplexor + 1) % 4;
	 
	 // Increase sensitivity for the selected parameter.
	 if (display_param == analog_params[analogMultiplexor]) {
		 change_threshold = LOW_CHANGE_THRESHOLD;
	 } else {
		 change_threshold = DEFAULT_CHANGE_THRESHOLD;
	 }
	 
	 readInput(analogMultiplexor, shift_state, editing);
 }
 
 /**
  * @brief Poll the analog input for calibration.
  *
  * Uses a lower threshold for detecting changes.
  */
 void AnalogIo::pollCalibration() {
	 change_threshold = LOW_CHANGE_THRESHOLD;
	 param_changed = readInput(3, false, false);
 }
 
 /**
  * @brief Read and process an analog input.
  *
  * Reads the analog input on index 'i' and, if the change is above threshold,
  * processes it depending on whether writing of values is enabled.
  *
  * @param i Index of the analog input.
  * @param shift_state Current state of the shift button.
  * @param write_values If true, update system values based on input.
  * @return true if a significant change was detected, false otherwise.
  */
 bool AnalogIo::readInput(int i, bool shift_state, bool write_values) {
	 // Read the analog value for channel i.
	 analogValues[i] = analogRead(analog_pins[i]);
	 param_changed = false;
	 
	 // If the change is larger than the threshold, process the value.
	 if (abs(analogValues[i] - lastAnalogValues[i]) > (shift_state ? SHIFT_CHANGE_THRESHOLD : change_threshold)) {
		 recorded_input_active = true;
		 lastAnalogValues[i] = analogValues[i];
		 
		 if (!write_values) {
			 return true;
		 }
		 
		 switch (i) {
			 case PITCH_PARAM:
				 // When shift is held, adjust audition mode instead of pitch.
				 shift_state ? setAudition(analogValues[0]) : setPitch(analogValues[0]);
				 break;
			 case OCTAVE_PARAM:
				 setOctave(analogValues[1]);
				 break;
			 case DURATION_PARAM:
				 setDuration(analogValues[2]);
				 break;
			 case CV_PARAM:
				 // When shift is held, adjust CV mode; otherwise, adjust CV output.
				 shift_state ? setCVMode(analogValues[3]) : setCV(analogValues[3]);
				 break;
		 }
		 return true;
	 }
	 return false;
 }
 
 /**
  * @brief Process the pitch parameter from analog input.
  *
  * Converts the analog value to a pitch, updates the display, and triggers
  * auditioning if enabled.
  *
  * @param analogValue Raw analog input value.
  */
 void AnalogIo::setPitch(int analogValue) {
	 display_param = PITCH_PARAM;
	 
	 // Convert analog value (0-1023) to a pitch range (-12 to +12)
	 int newVal = analogValue / 42.1 - 12.1;
	 
	 if (recording || sequencerVar->setPitch(newVal)) {
		 setDisplayNum(newVal);
		 displayPitchName(newVal, sequencerVar->getOctave());
		 if (audition) {
			 sequencerVar->auditionNote(true, 120);
		 }
	 }
 }
 
 /**
  * @brief Process the octave parameter from analog input.
  *
  * Converts the analog value to an octave, updates the display,
  * and triggers auditioning if enabled.
  *
  * @param analogValue Raw analog input value.
  */
 void AnalogIo::setOctave(int analogValue) {
	 display_param = OCTAVE_PARAM;
	 
	 // Convert analog value to octave range (-4 to +4)
	 int newVal = analogValue / 120 - 4;
	 
	 if (recording || sequencerVar->setOctave(newVal)) {
		 setDisplayNum(newVal);
		 displayPitchName(sequencerVar->getPitch(), newVal);
		 if (audition) {
			 sequencerVar->auditionNote(true, 120);
		 }
	 }
 }
 
 /**
  * @brief Process the duration parameter from analog input.
  *
  * Applies an exponential curve to the analog value and updates the display.
  *
  * @param analogValue Raw analog input value.
  */
 void AnalogIo::setDuration(long analogValue) {
	 display_param = DURATION_PARAM;
	 
	 // Convert analog value to a duration value with an exponential curve.
	 int newVal = analogValue * analogValue / 2615;
	 
	 if (recording || sequencerVar->setDuration(newVal)) {
		 setDisplayNum(newVal);
	 }
 }
 
 /**
  * @brief Process the CV parameter from analog input.
  *
  * Updates the CV output and the display based on the analog input.
  *
  * @param analogValue Raw analog input value.
  */
 void AnalogIo::setCV(int analogValue) {
	 display_param = CV_PARAM;
	 
	 if (recording || sequencerVar->setCv2(analogValue)) {
		 int newVal = sequencerVar->getCv2DisplayValue(analogValue);
		 setDisplayNum(newVal);
		 displayCvName(newVal);
	 }
 }
 
 /**
  * @brief Update the display with the currently selected parameter.
  *
  * Chooses the appropriate display function based on the current parameter.
  */
 void AnalogIo::displaySelectedParam() {
	 switch (display_param) {
		 case PITCH_PARAM:
			 setDisplayNum(sequencerVar->getPitch());
			 displayPitchName(sequencerVar->getPitch(), sequencerVar->getOctave());
			 break;
		 case OCTAVE_PARAM:
			 setDisplayNum(sequencerVar->getOctave());
			 displayPitchName(sequencerVar->getPitch(), sequencerVar->getOctave());
			 break;
		 case DURATION_PARAM:
			 setDisplayNum(sequencerVar->getDuration());
			 break;
		 case CV_PARAM:
			 setDisplayNum(sequencerVar->getCv());
			 displayCvName(sequencerVar->getCv());
			 break;
	 }
 }
 
 /**
  * @brief Set the numeric display value.
  *
  * @param displayNum Numeric value to be shown on the display.
  */
 void AnalogIo::setDisplayNum(int displayNum) {
	 display_num = displayNum;
	 param_changed = true;
 }
 
 /**
  * @brief Set the alphanumeric display string.
  *
  * Copies a 3-character string into the display_alpha buffer.
  *
  * @param displayAlpha Character array containing the display string.
  */
 void AnalogIo::setDisplayAlpha(char displayAlpha[]) {
	 // Copy exactly three characters.
	 for (int i = 0; i < 3; i++) {
		 display_alpha[i] = displayAlpha[i];
	 }
	 param_changed = true;
 }
 
 /**
  * @brief Display the pitch name on the alphanumeric display.
  *
  * Converts pitch and octave into a note name string and updates the display.
  *
  * @param pitch The pitch value.
  * @param octave The octave value.
  */
 void AnalogIo::displayPitchName(int pitch, int octave) {
	 setDisplayAlpha(sequencerVar->getPitchName(sequencerVar->getMidiPitch(pitch, octave)));
 }
 
 /**
  * @brief Display the CV mode name on the alphanumeric display.
  *
  * If the CV mode is set to NOTE MODE, displays the corresponding note name.
  *
  * @param pitch The pitch value for calculating the note name.
  */
 void AnalogIo::displayCvName(int pitch) {
	 if (sequencerVar->getCvMode() == 3) { // NOTE MODE
		 setDisplayAlpha(sequencerVar->getPitchName(sequencerVar->getMidiPitch(pitch, -3)));
	 }
 }
 
 /**
  * @brief Record the current parameter based on analog input.
  *
  * If an input change has been detected, temporarily bypasses the recording flag
  * to update the current parameter.
  */
 void AnalogIo::recordCurrentParam() {
	 if (!recorded_input_active) return;
	 
	 // Temporarily disable recording to allow one-step record.
	 recording = false;
	 change_threshold = -1; // Force recording regardless of change magnitude.
	 readInput(display_param, false, true);
	 recording = true;
 }
 
 /**
  * @brief Set the record mode state.
  *
  * @param state true to enable recording, false to disable.
  */
 void AnalogIo::setRecordMode(bool state) {
	 recorded_input_active = false;
	 recording = state;
 }
 
 /**
  * @brief Set the display mode.
  *
  * @param mode Mode value (e.g. numeric or note name).
  */
 void AnalogIo::setDisplayMode(int mode) {
	 display_mode = mode;
 }
 
 /**
  * @brief Get the current numeric display value.
  *
  * @return int Numeric display value.
  */
 int AnalogIo::getDisplayNum() {
	 return display_num;
 }
 
 /**
  * @brief Get the current alphanumeric display string.
  *
  * @return char* Pointer to the alphanumeric display buffer.
  */
 char* AnalogIo::getDisplayAlpha() {
	 return display_alpha;
 }
 
 /**
  * @brief Check if a parameter change has occurred.
  *
  * @return true if the parameter has changed, false otherwise.
  */
 bool AnalogIo::paramChanged() {
	 return param_changed;
 }
 
 /**
  * @brief Determine if the parameter should be displayed as an alphanumeric string.
  *
  * Combines display mode and the current parameter selection to decide.
  *
  * @return true if the parameter is alphanumeric, false if numeric.
  */
 bool AnalogIo::paramIsAlpha() {
	 return ((display_mode == DISPLAY_MODE_NOTE_NAME) && (display_param == PITCH_PARAM || display_param == OCTAVE_PARAM)) ||
			(display_param == MODE_PARAM) ||
			(display_param == CV_PARAM && sequencerVar->getCvMode() == 3);
 }
 
 /**
  * @brief Set the CV mode based on the analog input.
  *
  * Determines the CV mode from the analog value, retrieves the mode name from PROGMEM,
  * updates the display, and sets the CV mode in the sequencer.
  *
  * @param analogValue Raw analog input value.
  */
 void AnalogIo::setCVMode(int analogValue) {
	 display_param = MODE_PARAM;
	 uint8_t cv_mode = 0;
	 
	 if (analogValue < 256) {
		 cv_mode = 0;
	 } else if (analogValue < 512) {
		 cv_mode = 1;
	 } else if (analogValue < 768) {
		 cv_mode = 2;
	 } else {
		 cv_mode = 3;
	 }
	 
	 // Copy the corresponding mode name from PROGMEM.
	 strcpy_P(modename, (char *)pgm_read_word(&(cvmode_names[cv_mode])));
	 setDisplayAlpha(modename);
	 sequencerVar->setCVMode(cv_mode);
	 param_changed = true;
 }
 
 /**
  * @brief Set the audition mode based on the analog input.
  *
  * If the analog value exceeds a threshold, enable audition mode and update the display.
  *
  * @param analogValue Raw analog input value.
  */
 void AnalogIo::setAudition(int analogValue) {
	 display_param = MODE_PARAM;
	 
	 if (analogValue > 900) {
		 audition = true;
		 strcpy_P(modename, (char *)pgm_read_word(&(audition_names[1])));
	 } else {
		 strcpy_P(modename, (char *)pgm_read_word(&(audition_names[0])));
		 audition = false;
		 // Stop any ongoing audition note.
		 sequencerVar->auditionNote(false, 1);
	 }
	 
	 setDisplayAlpha(modename);
	 param_changed = true;
 }
 
 /**
  * @brief Get the calibration value from the analog input.
  *
  * Converts the raw analog value (from channel 3) into a calibration value.
  *
  * @return int Calibration value.
  */
 int AnalogIo::getCalibrationValue() {
	 int calibration_value = (analogValues[3] - 512) / 10;
	 setDisplayNum(calibration_value);
	 return calibration_value;
 }
 
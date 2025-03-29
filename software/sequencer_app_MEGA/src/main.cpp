#include <Arduino.h>
#include <string.h>
#include <SPI.h>
#include <elapsedMillis.h>

#include "Buttons.h"
#include "Encoder.h"
#include "AnalogIO.h"
#include "Dac.h"
#include "Display.h"
#include "Font.h"
#include "LEDMatrix.h"
#include "Main.h"
#include "Sequencer.h"
#include "Ui.h"
#include "Dac.h"
#include "Calibrate.h"
#include "Pinout.h"

Ui ui;
Dac dac;
Calibration calibration;
Sequencer sequencer;
// Display disp;

void setup() {
	analogReference(EXTERNAL); // use AREF for reference voltage

	calibration.readCalibrationValues(); // -- disable to bypass overwriting EEPROM during programming/development. uncomment for typical use
	
    sequencer.init(calibration, dac);
	ui.init(calibration, dac, sequencer);
  // disp.init(DIO_PIN, RCLK_PIN, SCLK_PIN, DIGIT_1_PIN, DIGIT_2_PIN, DIGIT_3_PIN);

}


void loop() {
	ui.poll();
	ui.multiplex();
	if (ui.isSequencing()){
		run_sequence();
	}
  // disp.update();
  // disp.setDisplayAlpha("MEM");
  
}

void run_sequence() {
	sequencer.updateClock();
	if (sequencer.stepWasIncremented()){
		ui.onStepIncremented();
	} 

}

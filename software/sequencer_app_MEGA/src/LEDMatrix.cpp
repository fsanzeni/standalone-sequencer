#include <Arduino.h>
#include <elapsedMillis.h>
#include <Adafruit_NeoPixel.h>

#include "pinout.h"
#include "display.h"
#include "sequencer.h"
#include "LEDMatrix.h"
#include "buttons.h"
#include "font.h"

//-----------------------------------------------------------------------------
// Global Variables for the LED Matrix (NeoPixel version)

// Assume a matrix of 16 NeoPixels (4 rows × 4 columns)
static const int NUM_PIXELS = 16;

// Create a NeoPixel object on a pin defined in pinout.h
// Adjust the color order (here NEO_GRB) as required.
static Adafruit_NeoPixel neoMatrix = Adafruit_NeoPixel(NUM_PIXELS, LED_MATRIX_PIN, NEO_GRB + NEO_KHZ800);

// LED state array: each element is a boolean indicating whether that LED is ON (true) or OFF (false).
static bool led_matrix[16] = { false };

// Mapping array (from logical step to LED index).
static int step_map[16] = {
    0, 1, 2, 3, 4, 5, 6, 7,  // Row 1
    7, 6, 5, 4, 3, 2, 1, 0   // Row 2
};
// The index of the currently selected step (for blinking, etc.)
static int selected_step_led = 0;

// Visible bar index (if your sequencer organizes steps into bars)
static byte visible_bar = 0;

// Pointers to external Display and Sequencer objects.
static Display* displayVar3 = nullptr;
static Sequencer* sequencerVar3 = nullptr;

// Timing variables for blinking.
elapsedMillis blinker;

//-----------------------------------------------------------------------------
// LEDMatrix Class Member Functions
//-----------------------------------------------------------------------------

// Initialize the LED matrix. 
// With NeoPixels, we initialize the NeoPixel object and store pointers to the Display and Sequencer.
void LedMatrix::init(Display& display, Sequencer& sequencer) {
    // Initialize the NeoPixel matrix.
    neoMatrix.begin();
    neoMatrix.clear();
    neoMatrix.show();

    // Store references to external objects.
    displayVar3 = &display;
    sequencerVar3 = &sequencer;
}

//-----------------------------------------------------------------------------
// Update the LED matrix.
// Update every NeoPixel based on the led_matrix boolean array.
// For each LED that is ON, set a white color; OFF gives black.
void LedMatrix::updateMatrix(int /*row*/) {
    // For NeoPixels, update all pixels at once.
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t color = led_matrix[i] ? neoMatrix.Color(255, 255, 255) : neoMatrix.Color(0, 0, 0);
        neoMatrix.setPixelColor(i, color);
    }
    neoMatrix.show();

    // Update the 7-segment display as part of the overall update cycle.
    if (displayVar3) {
        displayVar3->update();
        displayVar3->nextDigit();
    }
}

//-----------------------------------------------------------------------------
// Blank the LED matrix by turning off all NeoPixels.
// Also, blank the 7-segment display.
void LedMatrix::blankMatrix(int /*row*/) {
    neoMatrix.clear();
    neoMatrix.show();

    if (displayVar3) {
        displayVar3->blankSevenSegmentDisplay();
    }
}

//-----------------------------------------------------------------------------
// Multiplexing function.
// With individually addressable NeoPixels, multiplexing isn’t required;
// however, if you wish to update at a regular interval, you can call updateMatrix() periodically.
void LedMatrix::multiplexLeds() {
    // Simply update the entire matrix periodically.
    // The original timing variables are preserved for compatibility.
    if (millis() % 2 == 0) {  // (Example: update every 2ms)
        updateMatrix(0);
    }
}

//-----------------------------------------------------------------------------
// Blink a step in the sequence based on sequencer state.
// This function toggles the LED at the selected step when a threshold is reached.
void LedMatrix::blinkStep() {
    bool stepActive = sequencerVar3->getStepOnOff(selected_step_led + visible_bar * 16);
    // Use different blink durations depending on whether the step is active.
    if (blinker > (led_matrix[selected_step_led] ? 600 : 100)) {
        blinkLed();
    }
}

//-----------------------------------------------------------------------------
// Toggle (blink) the LED at the currently selected step.
void LedMatrix::blinkLed() {
    led_matrix[selected_step_led] = !led_matrix[selected_step_led];
    blinker = 0;
    updateMatrix(0);
}

//-----------------------------------------------------------------------------
// Blink the current step based on the sequencer's current step.
// If the current step falls within the visible bar, toggle its state.
void LedMatrix::blinkCurrentStep() {
    int current_step = sequencerVar3->getCurrentStep();
    int visible_step = current_step % 16;
    if (current_step >= visible_bar * 16 && current_step < (visible_bar + 1) * 16) {
        led_matrix[visible_step] = !led_matrix[visible_step];
        updateMatrix(0);
    }
}

//-----------------------------------------------------------------------------
// Reset the LED matrix by turning all LEDs off.
void LedMatrix::reset() {
    for (int i = 0; i < 16; i++) {
        led_matrix[i] = false;
    }
    neoMatrix.clear();
    neoMatrix.show();
}

//-----------------------------------------------------------------------------
// Select a specific step (LED index) to be the current step.
void LedMatrix::selectStep(int step) {
    selected_step_led = step;
}

//-----------------------------------------------------------------------------
// Set the matrix state from the sequencer's step matrix for a given bar.
// This copies 16 bytes from the sequencer into the local led_matrix array.
void LedMatrix::setMatrixFromSequencer(byte bar) {
    visible_bar = bar;
    memcpy(led_matrix, sequencerVar3->getStepMatrix() + bar * 16, 16);
    int selected = sequencerVar3->getSelectedStep();
    if (selected < bar * 16 || selected >= (bar + 1) * 16) {
        selected_step_led = 16; // Invalid step index in current bar.
    } else {
        selected_step_led = selected % 16;
    }
    updateMatrix(0);
}

//-----------------------------------------------------------------------------
// Set the LED matrix from an external boolean array (of length 16).
void LedMatrix::setMatrix(bool *matrix) {
    memcpy(led_matrix, matrix, 16);
    updateMatrix(0);
}

//-----------------------------------------------------------------------------
// Toggle the state of a specific LED in the matrix.
void LedMatrix::toggleLed(int led) {
    led_matrix[led] = !led_matrix[led];
    updateMatrix(0);
}

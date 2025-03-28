#include "pinout.h"
#include "buttons.h"
#include "calibrate.h"
#include "sequencer.h"
#include "queue_ino.h"
#include "buttonMap.h"
#include <elapsedMillis.h>
#include <Adafruit_NeoPixel.h>

//-----------------------------------------------------------------------------
// Global Variables & Objects for Button Handling

// Instantiate a NeoPixel object for the glide LED.
// GLIDE_NEOPIXEL_PIN is defined in pinout.h.
Adafruit_NeoPixel glideNeo = Adafruit_NeoPixel(1, GLIDE_NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Define an event queue with capacity 8 and element type uint16_t.
QUEUE(events, uint16_t, 8);
volatile struct queue_events queue;

bool editing_buttons = false;

// Debounce settings.
const byte BUTTON_DEBOUNCE_TIME = 10; // Debounce duration in milliseconds
elapsedMillis debounce_timer;           // Tracks time since last valid event
volatile uint16_t bouncing_button = 0x00; // Last button that triggered an event

// Button matrix state for 32 buttons (4 rows x 8 columns).
bool button_matrix[32] = { false };

// External mapping array to map physical button indices to logical IDs.
extern const int button_map[32];

//-----------------------------------------------------------------------------
// Buttons Class Member Functions
//-----------------------------------------------------------------------------

// Initialize the button matrix hardware and internal state.
void Buttons::init() {
    // Initialize row pins as OUTPUT and set them HIGH (inactive).
    for (int i = 0; i < 4; i++){
        pinMode(buttonRows[i], OUTPUT);
        digitalWrite(buttonRows[i], HIGH);
    }
    
    // Initialize column pins as INPUT_PULLUP.
    for (int i = 0; i < 8; i++){
        pinMode(buttonCols[i], INPUT_PULLUP);
    }
    
    // Initialize the NeoPixel for the glide LED.
    glideNeo.begin();
    glideNeo.clear();
    glideNeo.show();
    
    // Poll several times to stabilize the initial button states.
    poll();
    poll();
    poll();
    poll();
    
    editing_buttons = true;
}

// Poll the button matrix by scanning one row at a time.
// This function updates the state of each button and generates events when a button's state changes.
void Buttons::poll() {
    // Cycle through the 4 rows.
    row++;
    if (row > 3) {
        row = 0;
    }
    
    // Activate the current row by driving it LOW.
    digitalWrite(buttonRows[row], LOW);
    
    // For each of the 8 columns in the current row:
    for (int col = 0; col < 8; col++){
        int buttonIndex = row * 8 + col;  // Calculate physical index in 4x8 matrix
        // With INPUT_PULLUP, a pressed button pulls the line LOW.
        bool pressed = (digitalRead(buttonCols[col]) == LOW);
        
        // Map the physical button index to a logical button ID.
        int mappedButton = button_map[buttonIndex];
        
        // If the button state has changed:
        if (pressed != button_matrix[buttonIndex]) {
            // Update the matrix state.
            button_matrix[buttonIndex] = pressed;
            // Pack the event into a 16-bit value:
            // Lower 8 bits: mapped button ID; upper 8 bits: state (1 for pressed, 0 for released).
            uint16_t event = mappedButton | (pressed << 8);
            onButtonPush(event);
        }
    }
    
    // Deactivate the row by setting it back to HIGH.
    digitalWrite(buttonRows[row], HIGH);
}

// Called when a button is pushed (or released).
// Implements a simple debounce mechanism: if the same button triggers an event within BUTTON_DEBOUNCE_TIME,
// the event is ignored.
void Buttons::onButtonPush(uint16_t &event) {
    if (editing_buttons) {
        // Check if the same button is bouncing.
        if (debounce_timer < BUTTON_DEBOUNCE_TIME && (event & 0x00FF) == bouncing_button) {
            return;
        }
        // Push the event onto the queue.
        queue_events_push(&queue, &event);
        // Record which button triggered the event.
        bouncing_button = event & 0x00FF;
        debounce_timer = 0;
    }
}

// Retrieve a queued button event.
// Returns an event code and stores its value in 'value'.
int Buttons::getQueuedEvent(uint16_t &value) {
    return queue_events_pop(&queue, &value);
}

// Update the state of the glide LED using the NeoPixel.
// When glide is true, the LED is set to white; otherwise, it is turned off.
void Buttons::setGlideLed(bool glide) {
    if (glide) {
        glideNeo.setPixelColor(0, glideNeo.Color(255, 255, 255)); // White color
    } else {
        glideNeo.setPixelColor(0, 0); // Turn off
    }
    glideNeo.show();
}

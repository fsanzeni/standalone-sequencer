/**
 * @file buttons.cpp
 * @brief Button handling implementation for the Arduino sequencer.
 *
 * This file implements scanning of a 4x8 button matrix, debouncing,
 * event queue handling, and control of a glide LED via NeoPixel.
 */

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
 //-----------------------------------------------------------------------------
 
 // Instantiate a NeoPixel object for the glide LED.
 // GLIDE_NEOPIXEL_PIN is defined in pinout.h.
 Adafruit_NeoPixel glideNeo = Adafruit_NeoPixel(1, GLIDE_NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
 
 // Define an event queue with capacity 8 for uint16_t events.
 QUEUE(events, uint16_t, 8);
 volatile struct queue_events queue;
 
 // Flag to indicate if button editing (initialization) is complete.
 bool editing_buttons = false;
 
 // Debounce settings.
 const byte BUTTON_DEBOUNCE_TIME = 10; // Debounce duration in milliseconds.
 elapsedMillis debounce_timer;           // Timer to track debounce period.
 volatile uint16_t bouncing_button = 0x00; // Last button (logical ID) that triggered an event.
 
 // Button matrix state for 32 buttons (4 rows x 8 columns).
 bool button_matrix[32] = { false };
 
 // External mapping array to map physical button indices to logical IDs.
 extern const int button_map[32];
 
 //-----------------------------------------------------------------------------
 // Buttons Class Member Functions
 //-----------------------------------------------------------------------------
 
 /**
  * @brief Initialize the button matrix hardware and internal state.
  *
  * This function sets up row pins as outputs (initially HIGH) and column pins as
  * INPUT_PULLUP. It also initializes the glide LED (NeoPixel) and polls the button
  * matrix several times to stabilize initial states.
  */
 void Buttons::init() {
     // Initialize row pins as OUTPUT and set them HIGH (inactive).
     for (int i = 0; i < 4; i++) {
         pinMode(buttonRows[i], OUTPUT);
         digitalWrite(buttonRows[i], HIGH);
     }
     
     // Initialize column pins as INPUT_PULLUP.
     for (int i = 0; i < 8; i++) {
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
 
 /**
  * @brief Poll the button matrix.
  *
  * Scans one row of the button matrix at a time, updating the state of each button.
  * If a button state change is detected, an event is generated.
  */
 void Buttons::poll() {
     // Cycle through the 4 rows.
     row = (row + 1) % 4;
     
     // Activate the current row by driving it LOW.
     digitalWrite(buttonRows[row], LOW);
     
     // Scan each column in the current row.
     for (int col = 0; col < 8; col++) {
         int buttonIndex = row * 8 + col;  // Calculate physical index in the 4x8 matrix.
         // With INPUT_PULLUP, a pressed button reads LOW.
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
     
     // Deactivate the current row by setting it back to HIGH.
     digitalWrite(buttonRows[row], HIGH);
 }
 
 /**
  * @brief Handle a button push event with debounce.
  *
  * If the same button triggers an event within the debounce period, the event is ignored.
  * Otherwise, the event is added to the event queue.
  *
  * @param event Reference to the 16-bit event value.
  */
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
 
 /**
  * @brief Retrieve a queued button event.
  *
  * Removes and returns an event from the queue.
  *
  * @param value Reference to store the event value.
  * @return int Number of events in the queue after the pop (or an error code).
  */
 int Buttons::getQueuedEvent(uint16_t &value) {
     return queue_events_pop(&queue, &value);
 }
 
 /**
  * @brief Update the state of the glide LED.
  *
  * Sets the NeoPixel LED to white when glide mode is active, and turns it off otherwise.
  *
  * @param glide true to turn the LED on (white), false to turn it off.
  */
 void Buttons::setGlideLed(bool glide) {
     if (glide) {
         glideNeo.setPixelColor(0, glideNeo.Color(255, 255, 255)); // White color.
     } else {
         glideNeo.setPixelColor(0, 0); // Turn off.
     }
     glideNeo.show();
 } 
#pragma once

#include "display.h"
#include "analogIO.h"

/*
 * Class: Buttons
 * ---------------------------
 *  Manages a matrix of buttons.
 *  It handles initialization, polling the button states,
 *  and generating events when buttons are pushed.
 */
class Buttons {
  public:
    // Constructor and Destructor
    Buttons() { }
    ~Buttons() { }
    
    // Initializes button pins, matrices, etc.
    void init();

    // Polls the button matrix and updates internal state.
    void poll();

    // Retrieves a queued event, storing the associated value in 'value'.
    // Returns an integer code representing the event.
    int getQueuedEvent(uint16_t &value);

    // Set the glide LED state.
    void setGlideLed(bool glide);

  private:
    // Select a step in the button matrix (helper for navigation/selection)
    void selectStep(unsigned int stepnum);

    // Individual button handlers (called when a particular button's state changes)
    void saveButton(bool state);
    void loadButton(bool state);
    void playButton(bool state);
    void shiftButton(bool state);
    void recordButton(bool state);
    void repeatButton(bool state);
    void glideButton(bool state);

    // Called when a button is pushed; event value is updated.
    void onButtonPush(uint16_t &event);

    // The current state of each button in the matrix (0 = not pressed, 1 = pressed).
    bool button_matrix[16] = { false };

    // A counter used for the save button.
    int saveCount = 0;

    // Stores status of function buttons (if using a separate matrix for functions).
    // (Note: There is a workaround with an offset because the first bit never toggles.)
    bool function_button_matrix[16] = { false };

    // Variables used for scanning the matrix
    int row = 0;              // Current row being scanned
    uint16_t buttons_state;   // Combined state of all buttons (bitfield)
    uint16_t buttons_mask;    // Mask for active/pressed buttons
};

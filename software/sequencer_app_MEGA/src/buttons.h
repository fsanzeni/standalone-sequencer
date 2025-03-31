#pragma once

#include "display.h"
#include "analogIO.h"

/**
 * @brief Manages a matrix of buttons for the sequencer.
 *
 * This class is responsible for initializing the button hardware, scanning
 * the button matrix, and generating events when buttons are pushed. It also
 * includes helper methods for handling specific button functions.
 */
class Buttons {
  public:
    /**
     * @brief Default constructor.
     */
    Buttons() { }

    /**
     * @brief Destructor.
     */
    ~Buttons() { }
    
    /**
     * @brief Initialize the button matrix hardware and internal state.
     */
    void init();

    /**
     * @brief Poll the button matrix and update internal state.
     *
     * This method scans the matrix to detect button state changes.
     */
    void poll();

    /**
     * @brief Retrieve a queued button event.
     *
     * @param value Reference variable to store the event value.
     * @return int An integer code representing the event.
     */
    int getQueuedEvent(uint16_t &value);

    /**
     * @brief Set the state of the glide LED.
     *
     * @param glide true to turn the LED on, false to turn it off.
     */
    void setGlideLed(bool glide);

  private:
    /**
     * @brief Helper function to select a step based on the button matrix.
     *
     * @param stepnum The step number to be selected.
     */
    void selectStep(unsigned int stepnum);

    // Individual button handler functions, called when a specific button's state changes.
    void saveButton(bool state);
    void loadButton(bool state);
    void playButton(bool state);
    void shiftButton(bool state);
    void recordButton(bool state);
    void repeatButton(bool state);
    void glideButton(bool state);

    /**
     * @brief Called when a button is pushed (or released).
     *
     * Processes the event and updates the event value.
     *
     * @param event Reference to the event value.
     */
    void onButtonPush(uint16_t &event);

    // State of the button matrix (16 buttons) for navigation/selection.
    bool button_matrix[16] = { false };

    // Counter used for the save button (for example, to track multiple presses).
    int saveCount = 0;

    // State for function buttons, using an offset workaround for a toggling issue.
    bool function_button_matrix[16] = { false };

    // Variables used for scanning the matrix.
    int row = 0;              // Current row being scanned.
    uint16_t buttons_state;   // Combined state of all buttons (bitfield).
    uint16_t buttons_mask;    // Mask for determining active/pressed buttons.
};

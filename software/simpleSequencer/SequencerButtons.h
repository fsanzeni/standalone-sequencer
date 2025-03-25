#ifndef SEQUENCER_BUTTONS_H
#define SEQUENCER_BUTTONS_H

#include <Arduino.h>

// The number of function buttons.
#define NUM_FUNCTION_BUTTONS 7

// Debounce time in milliseconds.
#define BUTTON_DEBOUNCE_TIME 10

// Size of the event queue.
#define QUEUE_SIZE 8

class SequencerButtons {
public:
  /**
   * @brief Construct a new SequencerButtons object.
   *
   * @param functionPins  An array of seven pin numbers assigned to the function buttons.
   * @param glideLedPin   The pin number for the glide LED.
   */
  SequencerButtons(const int functionPins[NUM_FUNCTION_BUTTONS], int glideLedPin);

  /**
   * @brief Initialize the button pins and the glide LED.
   *
   * This method also primes the button states and performs a few initial polls.
   */
  void init();

  /**
   * @brief Poll the button pins and check for state changes.
   *
   * If a button’s state has changed (and passes debouncing) an event is pushed into
   * the queue. The event encodes both the button identifier (lower 8 bits) and state (upper 8 bits).
   */
  void poll();

  /**
   * @brief Retrieve an event from the queue.
   *
   * @param value Will contain the 16-bit encoded event if one exists.
   * @return int 1 if an event was available, 0 if the queue is empty.
   */
  int getQueuedEvent(uint16_t &value);

  /**
   * @brief Set the state of the glide LED.
   *
   * @param state true for HIGH (on), false for LOW (off)
   */
  void setGlideLed(bool state);

private:
  int _functionPins[NUM_FUNCTION_BUTTONS]; // Pins for the function buttons.
  int _glideLedPin;                        // Pin for the glide LED.
  bool _buttonStates[NUM_FUNCTION_BUTTONS];  // Last known state for each button.

  // A simple circular buffer for events.
  volatile uint16_t _queue[QUEUE_SIZE];
  volatile int _queueHead;
  volatile int _queueTail;

  // Editing mode flag and debounce variables.
  bool _editingButtons;
  unsigned long _lastDebounceTime;
  volatile uint16_t _bouncingButton;

  /**
   * @brief Push an event into the queue.
   *
   * @param event 16-bit encoded event.
   */
  void pushQueue(uint16_t event);

  /**
   * @brief Pop an event from the queue.
   *
   * @param value Will contain the popped event.
   * @return int 1 if an event was popped, 0 if the queue is empty.
   */
  int popQueue(uint16_t &value);

  /**
   * @brief Process a button push event (with debouncing) and add it to the queue.
   *
   * @param event 16-bit encoded event.
   */
  void onButtonPush(uint16_t event);
};

#endif

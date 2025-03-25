#include "SequencerButtons.h"

SequencerButtons::SequencerButtons(const int functionPins[NUM_FUNCTION_BUTTONS], int glideLedPin)
  : _glideLedPin(glideLedPin),
    _queueHead(0),
    _queueTail(0),
    _editingButtons(false),
    _lastDebounceTime(0),
    _bouncingButton(0)
{
  // Copy the function pin numbers and initialize the button states to false.
  for (int i = 0; i < NUM_FUNCTION_BUTTONS; i++) {
    _functionPins[i] = functionPins[i];
    _buttonStates[i] = false;
  }
}

void SequencerButtons::init() {
  // Set each function button pin as INPUT_PULLUP and read its initial state.
  for (int i = 0; i < NUM_FUNCTION_BUTTONS; i++) {
    pinMode(_functionPins[i], INPUT_PULLUP);
    _buttonStates[i] = (digitalRead(_functionPins[i]) == LOW);
  }
  // Initialize the glide LED.
  pinMode(_glideLedPin, OUTPUT);
  digitalWrite(_glideLedPin, LOW);

  // Prime the button states by polling several times.
  poll();
  poll();
  poll();
  poll();
  
  _editingButtons = true;
}

void SequencerButtons::poll() {
  // Iterate through each function button.
  for (int i = 0; i < NUM_FUNCTION_BUTTONS; i++) {
    // Read the current state (assuming active LOW).
    bool value = (digitalRead(_functionPins[i]) == LOW);
    // Check for a state change.
    if (value != _buttonStates[i]) {
      _buttonStates[i] = value;
      // Create an event: lower 8 bits are (pin number + 8) and upper 8 bits are the state (1 for pressed).
      uint16_t event = ((uint16_t)(_functionPins[i] + 8) & 0xFF) | ((value ? 1 : 0) << 8);
      onButtonPush(event);
    }
  }
}

void SequencerButtons::onButtonPush(uint16_t event) {
  if (_editingButtons) {
    unsigned long now = millis();
    // If within the debounce time and the same button is bouncing, ignore this event.
    if ((now - _lastDebounceTime < BUTTON_DEBOUNCE_TIME) && ((event & 0x00FF) == _bouncingButton)) {
      return;
    }
    pushQueue(event);
    _bouncingButton = event & 0x00FF;
    _lastDebounceTime = now;
  }
}

void SequencerButtons::pushQueue(uint16_t event) {
  int nextHead = (_queueHead + 1) % QUEUE_SIZE;
  // Only push if the queue is not full.
  if (nextHead != _queueTail) {
    _queue[_queueHead] = event;
    _queueHead = nextHead;
  }
}

int SequencerButtons::popQueue(uint16_t &value) {
  if (_queueHead == _queueTail) {
    return 0; // Queue is empty.
  }
  value = _queue[_queueTail];
  _queueTail = (_queueTail + 1) % QUEUE_SIZE;
  return 1;
}

int SequencerButtons::getQueuedEvent(uint16_t &value) {
  return popQueue(value);
}

void SequencerButtons::setGlideLed(bool state) {
  digitalWrite(_glideLedPin, state ? HIGH : LOW);
}
#ifndef SEQUENCER_KEYBOARD_H
#define SEQUENCER_KEYBOARD_H

#include <Arduino.h>
#include <Keypad.h>

class SequencerKeyboard {
public:
  // Make these available publicly so they can be accessed externally.
  static const byte ROWS = 4;
  static const byte COLS = 8;

  // Define key codes for normal and alternate functions.
  enum KeyCode {
    KEY_NONE,
    KEY_SHIFT,
    KEY_PLAY,
    KEY_LOAD,
    KEY_SAVE,
    KEY_RECORD,
    KEY_MUTATE,
    KEY_TYPE,         // Alternate for MUTATE when shift is active
    KEY_GLIDE,
    KEY_GLIDE_LENGTH, // Alternate for GLIDE when shift is active
    KEY_STEP,         // Generic step key (use 'step' field in event)
    KEY_TEMPO,        // Alternate for step 9 (with shift)
    KEY_STEPS,        // Alternate for step 10 (with shift)
    KEY_SCALE,        // Alternate for step 11 (with shift)
    KEY_SWING,        // Alternate for step 12 (with shift)
    KEY_TRANSPOSE,    // Alternate for step 13 (with shift)
    KEY_CALIBRATE,    // Alternate for step 14 (with shift)
    KEY_CLEAR         // Alternate for step 15 (with shift)
  };

  // Struct used to return a key event.
  struct KeyEvent {
    KeyCode code;
    uint8_t step; // if code==KEY_STEP then step is 1-indexed step number (1-16)
  };

  // Default constructor uses the built-in pin and keymap definitions.
  SequencerKeyboard();

  // Alternate constructor to supply custom row/column pins, keymap and analog pins.
  // The keymap must be a 4x8 array.
  SequencerKeyboard(const byte rowPins[4],
                    const byte colPins[8],
                    const char keymap[4][8],
                    int pitchPin,
                    int octavePin,
                    int lengthPin,
                    int cvPin);

  // Initialize the keypad and analog pins.
  void begin();

  // Call this in your main loop. It polls the keypad and returns a KeyEvent.
  // If no key is pressed, the returned event has code KEY_NONE.
  KeyEvent getKeyEvent();

  // Read potentiometer values.
  int readPitch();   // PITCH
  int readOctave();  // OCTAVE
  int readLength();  // LENGTH
  int readCV();      // CV

private:
  byte _rowPins[ROWS];
  byte _colPins[COLS];
  char _keymap[ROWS][COLS];

  // Analog pin assignments.
  int _pitchPin;
  int _octavePin;
  int _lengthPin;
  int _cvPin;

  // Pointer to the Keypad object.
  Keypad* _keypad;

  // Used to track shift state.
  bool _shiftActive;

  // Helper function that maps a raw key (char) and the current shift state into a KeyEvent.
  KeyEvent mapKey(char key, bool shiftActive);
};

#endif

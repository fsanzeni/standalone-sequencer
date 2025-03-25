#include "SequencerKeyboard.h"

// Default definitions for pins and keymap
static const byte DEFAULT_ROW_PINS[SequencerKeyboard::ROWS] = { 46, 48, 50, 52 };
static const byte DEFAULT_COL_PINS[SequencerKeyboard::COLS] = { 39, 41, 43, 45, 47, 49, 51, 53 };
static const char DEFAULT_KEYMAP[SequencerKeyboard::ROWS][SequencerKeyboard::COLS] = {
  { '1', '2', '3', 'A', 'B', 'C', 'D', 'E' },
  { '4', '5', '6', 'F', 'G', 'H', 'I', 'J' },
  { '7', '8', '9', 'K', 'L', 'M', 'N', 'O' },
  { '*', '0', '#', 'P', 'Q', 'R', 'S', 'T' }
};

// Default analog pin assignments
static const int DEFAULT_PITCH_PIN  = A0;
static const int DEFAULT_OCTAVE_PIN = A1;
static const int DEFAULT_LENGTH_PIN = A2;
static const int DEFAULT_CV_PIN     = A3;

// --- Constructors ---

SequencerKeyboard::SequencerKeyboard()
  : _pitchPin(DEFAULT_PITCH_PIN),
    _octavePin(DEFAULT_OCTAVE_PIN),
    _lengthPin(DEFAULT_LENGTH_PIN),
    _cvPin(DEFAULT_CV_PIN),
    _shiftActive(false),
    _keypad(nullptr)
{
  // Copy default row pins and col pins.
  for (byte i = 0; i < ROWS; i++) {
    _rowPins[i] = DEFAULT_ROW_PINS[i];
  }
  for (byte i = 0; i < COLS; i++) {
    _colPins[i] = DEFAULT_COL_PINS[i];
  }
  // Copy the default keymap.
  for (byte r = 0; r < ROWS; r++) {
    for (byte c = 0; c < COLS; c++) {
      _keymap[r][c] = DEFAULT_KEYMAP[r][c];
    }
  }
}

SequencerKeyboard::SequencerKeyboard(const byte rowPins[4],
                                     const byte colPins[8],
                                     const char keymap[4][8],
                                     int pitchPin,
                                     int octavePin,
                                     int lengthPin,
                                     int cvPin)
  : _pitchPin(pitchPin),
    _octavePin(octavePin),
    _lengthPin(lengthPin),
    _cvPin(cvPin),
    _shiftActive(false),
    _keypad(nullptr)
{
  for (byte i = 0; i < ROWS; i++) {
    _rowPins[i] = rowPins[i];
  }
  for (byte i = 0; i < COLS; i++) {
    _colPins[i] = colPins[i];
  }
  for (byte r = 0; r < ROWS; r++) {
    for (byte c = 0; c < COLS; c++) {
      _keymap[r][c] = keymap[r][c];
    }
  }
}

// --- Public Methods ---

void SequencerKeyboard::begin() {
  // Instantiate the Keypad object.
  _keypad = new Keypad(makeKeymap(_keymap), _rowPins, _colPins, ROWS, COLS);
  // Setup analog pins as needed (on many boards this is automatic).
  pinMode(_pitchPin, INPUT);
  pinMode(_octavePin, INPUT);
  pinMode(_lengthPin, INPUT);
  pinMode(_cvPin, INPUT);
}

SequencerKeyboard::KeyEvent SequencerKeyboard::getKeyEvent() {
  KeyEvent event;
  event.code = KEY_NONE;
  event.step = 0;

  // Read a key from the keypad.
  char key = _keypad->getKey();

  // If no key is pressed, return no event.
  if (key == NO_KEY) {
    return event;
  }

  // If the key is the SHIFT key (assigned to '1') then
  // set the shift flag and return a SHIFT event.
  if (key == '1') {
    _shiftActive = true;
    event.code = KEY_SHIFT;
    return event;
  }
  
  // Otherwise, use the current shift state (if any) to map the key.
  event = mapKey(key, _shiftActive);

  // After using the shift modifier for a key, clear it.
  _shiftActive = false;
  
  return event;
}

int SequencerKeyboard::readPitch() {
  return analogRead(_pitchPin);
}

int SequencerKeyboard::readOctave() {
  return analogRead(_octavePin);
}

int SequencerKeyboard::readLength() {
  return analogRead(_lengthPin);
}

int SequencerKeyboard::readCV() {
  return analogRead(_cvPin);
}

// --- Private Helper ---

SequencerKeyboard::KeyEvent SequencerKeyboard::mapKey(char key, bool shiftActive) {
  KeyEvent event;
  event.code = KEY_NONE;
  event.step = 0;
  
  // Special function keys
  switch(key) {
    case '2':
      event.code = KEY_PLAY;
      break;
    case '3':
      event.code = KEY_LOAD;
      break;
    case 'A':
      event.code = KEY_SAVE;
      break;
    case '*':
      event.code = KEY_RECORD;
      break;
    case '0':
      event.code = (shiftActive ? KEY_TYPE : KEY_MUTATE);
      break;
    case '#':
      event.code = (shiftActive ? KEY_GLIDE_LENGTH : KEY_GLIDE);
      break;
      
    // Step keys
    // Row 1: keys for steps 1-3
    case '4': event.code = KEY_STEP; event.step = 1; break;
    case '5': event.code = KEY_STEP; event.step = 2; break;
    case '6': event.code = KEY_STEP; event.step = 3; break;
    // Row 1 continued: steps 4-8 (keys: F, G, H, I, J)
    case 'F': event.code = KEY_STEP; event.step = 4; break;
    case 'G': event.code = KEY_STEP; event.step = 5; break;
    case 'H': event.code = KEY_STEP; event.step = 6; break;
    case 'I': event.code = KEY_STEP; event.step = 7; break;
    case 'J': event.code = KEY_STEP; event.step = 8; break;
    
    // Row 2: keys for steps 9-16 (keys: 7,8,9,K,L,M,N,O)
    case '7':
      if (shiftActive)
        event.code = KEY_TEMPO;
      else {
        event.code = KEY_STEP;
        event.step = 9;
      }
      break;
    case '8':
      if (shiftActive)
        event.code = KEY_STEPS;
      else {
        event.code = KEY_STEP;
        event.step = 10;
      }
      break;
    case '9':
      if (shiftActive)
        event.code = KEY_SCALE;
      else {
        event.code = KEY_STEP;
        event.step = 11;
      }
      break;
    case 'K':
      if (shiftActive)
        event.code = KEY_SWING;
      else {
        event.code = KEY_STEP;
        event.step = 12;
      }
      break;
    case 'L':
      if (shiftActive)
        event.code = KEY_TRANSPOSE;
      else {
        event.code = KEY_STEP;
        event.step = 13;
      }
      break;
    case 'M':
      if (shiftActive)
        event.code = KEY_CALIBRATE;
      else {
        event.code = KEY_STEP;
        event.step = 14;
      }
      break;
    case 'N':
      if (shiftActive)
        event.code = KEY_CLEAR;
      else {
        event.code = KEY_STEP;
        event.step = 15;
      }
      break;
    case 'O':
      // Step 16 has no alternate function.
      event.code = KEY_STEP;
      event.step = 16;
      break;
      
    default:
      event.code = KEY_NONE;
      break;
  }
  
  return event;
}

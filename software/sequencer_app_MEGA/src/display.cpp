#include <Arduino.h>
#include <elapsedMillis.h>
#include "display.h"
#include "font.h"
#include "pinout.h"

// Global configuration variables for display behavior.
// These could be moved into the Display class if desired.
static bool decimal = false;
static const uint8_t DECIMAL_CHAR_MASK = 0x80; // Bit mask used for controlling the decimal point

static int digit_counter = 0;      // For managing digit multiplexing cycles
static int delay_counter = 0;      // Used to insert blank cycles (for brightness control)
static int brightness = 0;         // Brightness level (0 means always on)

static bool blinking = false;
// 
static bool blink_state = true;
static elapsedMillis display_blinker;  // Tracks elapsed time for blink control
static uint16_t blink_interval = 0;
static int blink_cycles_elapsed = 0;
static int blink_cycles_timeout = 0;

//=============================================================================
// Constructor: initialize numeric variables and clear digit arrays
Display::Display() : 
  _numDisplay(999),
  _currentDigit(0),
  _lastUpdate(0)
{
  // Initialize the digit and alphanumeric arrays to zero.
  for (int i = 0; i < 3; i++) {
    _digitDisplay[i] = 0;
    _alphaDisplay[i] = 0;
  }
}

//=============================================================================
// Initialize the shift register pins and digit control pins.
// digit1, digit2, digit3 correspond to the Arduino pins controlling the three digits.
void Display::init() {
  _DIO = DIO_PIN;
  _RCLK = RCLK_PIN;
  _SCLK = SCLK_PIN;
  _digitPins[0] = DIGIT_1_PIN;
  _digitPins[1] = DIGIT_2_PIN;
  _digitPins[2] = DIGIT_3_PIN;

  // Set up the shift register pins
  pinMode(_DIO, OUTPUT);
  pinMode(_RCLK, OUTPUT);
  pinMode(_SCLK, OUTPUT);
  
  // Set up the digit control pins and ensure they are turned off initially.
  for (int i = 0; i < 3; i++) {
    pinMode(_digitPins[i], OUTPUT);
    digitalWrite(_digitPins[i], HIGH);  // For common anode, HIGH turns the digit off.
  }
  setDisplayNum(0);
}

//=============================================================================
// Run a startup animation sequence that cycles through segments.
void Display::startupSequence() {
  // Each byte here lights one segment (active low/inverted for our font)
  byte segPatterns[7] = {
    0xFE,  // Only segment A on
    0xFD,  // Only segment B on
    0xFB,  // Only segment C on
    0xF7,  // Only segment D on
    0xEF,  // Only segment E on
    0xDF,  // Only segment F on
    0xBF   // Only segment G on
  };

  // Cycle through each segment pattern.
  for (int i = 0; i < 7; i++) {
    sendByte(segPatterns[i]);
    
    // Activate all three digits simultaneously.
    for (int d = 0; d < 3; d++) {
      digitalWrite(_digitPins[d], LOW);
    }
    
    delay(100);  // Display the pattern for 100ms
    
    // Turn off all digits.
    for (int d = 0; d < 3; d++) {
      digitalWrite(_digitPins[d], HIGH);
    }
    
    delay(50);  // Brief pause between segments
  }

  // Final flourish: light all segments (0x00 turns all segments on).
  sendByte(0x00);
  for (int d = 0; d < 3; d++) {
    digitalWrite(_digitPins[d], LOW);
  }
  delay(200);
  
  // Turn off all digits and clear the display.
  for (int d = 0; d < 3; d++) {
    digitalWrite(_digitPins[d], HIGH);
  }
  sendByte(0xFF);  // 0xFF turns all segments off
}

//=============================================================================
// Set the number to be displayed.
// This function decomposes the number into hundreds, tens, and units, handling negative numbers and leading zeros.
void Display::setDisplayNum(int displayNum) {
  if (_numDisplay == displayNum) return; // No change
  _numDisplay = displayNum;
  
  // For negative numbers, use a minus sign in the leftmost digit (assumed to be represented by index 37)
  if (_numDisplay < 0) {
    _digitDisplay[0] = 37; // Minus sign
    int absVal = abs(_numDisplay);
    _digitDisplay[1] = (absVal / 10) % 10;
    _digitDisplay[2] = absVal % 10;
    if (absVal < 10) _digitDisplay[1] = 36; // Blank leading zero for single-digit negative
  } else {
    _digitDisplay[0] = (_numDisplay / 100) % 10;
    _digitDisplay[1] = (_numDisplay / 10) % 10;
    _digitDisplay[2] = _numDisplay % 10;
    
    // Blank out leading zeros
    if (_numDisplay < 100) _digitDisplay[0] = 36;
    if (_numDisplay < 10) _digitDisplay[1] = 36;
  }
  
  // Set the alphanumeric display patterns from the font table.
  // (Assumes 'alphabet' is defined in font.h.)
  _alphaDisplay[0] = alphabet[_digitDisplay[0]];
  _alphaDisplay[1] = alphabet[_digitDisplay[1]];
  _alphaDisplay[2] = alphabet[_digitDisplay[2]];
  
  // Update the decimal point state.
  appendDecimal();
}

//=============================================================================
// The update() function handles multiplexing: it cycles through each digit,
// sends the corresponding segment data, and activates the digit briefly.
void Display::update() {
  // Use a non-blocking 2ms interval for updating the display.
  if (millis() - _lastUpdate >= 2) {
    _lastUpdate = millis();

    // Turn off all digits first.
    for (int i = 0; i < 3; i++) {
      digitalWrite(_digitPins[i], HIGH);
    }

    // Send segment data for the current digit.
    sendByte(_alphaDisplay[_currentDigit]);

    // Activate the current digit (active low).
    digitalWrite(_digitPins[_currentDigit], LOW);

    // Move to the next digit in a cyclic manner.
    _currentDigit = (_currentDigit + 1) % 3;
  }
}

//=============================================================================
// Set the alphanumeric display using a modifiable character array.
// This function creates a temporary constant array and calls setDisplayAlpha.
void Display::setDisplayAlphaVar(char displayAlpha[]){
  const char displayAlphaConst[4] = { displayAlpha[0], displayAlpha[1], displayAlpha[2] };
  setDisplayAlpha(displayAlphaConst);
}

//=============================================================================
// Set the alphanumeric display using a constant character array.
// Converts each character to an index into the font array, then stores the
// corresponding bit patterns.
void Display::setDisplayAlpha(const char displayAlpha[]){
  _numDisplay = 999; // Prime variable for easy reset
  for (int i = 0; i < 3; i++) {
    if (displayAlpha[i] == 98) { // if character is 'b'
      _digitDisplay[i] = 11; // Use font index for lowercase "b" (flat symbol)
    } else if (displayAlpha[i] == 32) { // Space
      _digitDisplay[i] = 36;
    } else if (displayAlpha[i] > 64) { // Convert A-Z to font index
      _digitDisplay[i] = displayAlpha[i] - 55;
    } else if (displayAlpha[i] >= 48) { // Convert 0-9 to index
      _digitDisplay[i] = displayAlpha[i] - 48;
    }
    // Store in reversed order (rightmost digit is first in alpha_display)
    _alphaDisplay[2 - i] = alphabet[_digitDisplay[i]];
  }
  appendDecimal();
}

//=============================================================================
// Send a byte to the shift register.
// This function pulses the latch pin to update the output.
void Display::sendByte(byte dataByte) {
  digitalWrite(_RCLK, LOW);
  shiftOut(_DIO, _SCLK, MSBFIRST, dataByte);
  digitalWrite(_RCLK, HIGH);
}

//=============================================================================
// Set the state of the decimal point (DP) and update the display.
// The DP is controlled in the first digit's data (alpha_display[0]).
void Display::setDecimal(bool decimalState) {
  decimal = decimalState;
  appendDecimal();
}

//=============================================================================
// Append (or remove) the decimal point from the display data.
// For active-low logic, clearing the DP bit turns it on.
void Display::appendDecimal() {
  if (decimal) {
    // Turn on the DP: clear the DP bit.
    _alphaDisplay[0] &= ~DECIMAL_CHAR_MASK;
  } else {
    // Turn off the DP: set the DP bit.
    _alphaDisplay[0] |= DECIMAL_CHAR_MASK;
  }
}

//=============================================================================
// Set the display brightness level.
// (The brightness logic is handled in currentDigitVisible() below.)
void Display::setBrightness(int new_brightness) {
  brightness = new_brightness;
}

//=============================================================================
// Determine whether the current digit should be visible based on brightness
// settings and delay counters.
bool Display::currentDigitVisible() {
  if (brightness == 0) { 
    return true; 
  } else if (brightness == 1) {
    if (delay_counter == _currentDigit) return false;
  } else if (brightness == 2) {
    if (delay_counter == 1) return false;
  } else if (brightness == 3) {
    if (delay_counter != _currentDigit) return false;
  } else if (brightness == 4) {
    if (delay_counter > 0) return false;
  }
  return true;
}

//=============================================================================
// Blank (turn off) the current digit on the 7-segment display.
// For a common anode display, setting the digit pin HIGH blanks that digit.
void Display::blankSevenSegmentDisplay() {
  digitalWrite(_digitPins[digit_counter], HIGH);
  // Optionally, call nextDigit() if you want to advance the digit.
  // nextDigit();
}

//=============================================================================
// Advance to the next digit (for multiplexing).
// This function updates the digit counter and handles brightness and blinking.
void Display::nextDigit() {
  digit_counter++;
  if (digit_counter > 2) {
    digit_counter = 0;
    delay_counter++;
    if (delay_counter > 2) {
      delay_counter = 0;
    }
  }
  if (blinking && display_blinker > blink_interval) {
    if (display_blinker > blink_interval * 2) {
      display_blinker = 0;
      blink_cycles_elapsed++;
      if (blink_cycles_timeout > 0 && blink_cycles_elapsed > blink_cycles_timeout) {
        blinking = false;
        blink_cycles_elapsed = 0;
      }
    }
    // In blinking mode, leave the digit inactive.
  } else {
    digitalWrite(_digitPins[digit_counter], HIGH); // Deactivate digit
  }
}

//=============================================================================
// Set the blink mode for the display.
void Display::blinkDisplay(bool is_blinking, int interval, int cycles) {
  blinking = is_blinking;
  display_blinker = 0;
  blink_interval = interval;
  blink_cycles_timeout = cycles;
}

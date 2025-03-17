#include "Arduino.h"
#include "sequencerDisplay.h"

// Lookup table for numbers on a common anode 7-seg display
// (Values are inverted from the typical common cathode codes)
byte numCodes[10] = {
  0xC0, // 0: segments A B C D E F on
  0xF9, // 1: segments B C on
  0xA4, // 2: segments A B D E G on
  0xB0, // 3: segments A B C D G on
  0x99, // 4: segments B C F G on
  0x92, // 5: segments A C D F G on
  0x82, // 6: segments A C D E F G on
  0xF8, // 7: segments A B C on
  0x80, // 8: all segments on
  0x90  // 9: segments A B C D F G on
};

// Constructor: initialize pins, digit control, and clear the display.
sequencerDisplay::sequencerDisplay(int DIO, int RCLK, int SCLK, int digit1, int digit2, int digit3) {
  _DIO = DIO;
  _RCLK = RCLK;
  _SCLK = SCLK;
  _digitPins[0] = digit1;
  _digitPins[1] = digit2;
  _digitPins[2] = digit3;
  
  // Set up the shift register pins
  pinMode(_DIO, OUTPUT);
  pinMode(_RCLK, OUTPUT);
  pinMode(_SCLK, OUTPUT);
  
  // Set up the digit control pins (active low)
  for (int i = 0; i < 3; i++) {
    pinMode(_digitPins[i], OUTPUT);
    digitalWrite(_digitPins[i], HIGH); // Keep digits off initially
  }
  
  // Initialize the digit buffer to blank (all segments off)
  for (int i = 0; i < 3; i++) {
    _digitBuffer[i] = 0xFF;
  }
  
  currentDigit = 0;
  Clear();
}

// Clear the display (both internal buffer and physical digits)
void sequencerDisplay::Clear() {
  for (int i = 0; i < 3; i++) {
    _digitBuffer[i] = 0xFF; // Blank
    digitalWrite(_digitPins[i], HIGH);
  }
  send_byte(0xFF);
}

// Set a number (0-9) to a specified digit (0 = left, 1 = middle, 2 = right)
void sequencerDisplay::Num(byte digit, byte n) {
  if (digit > 2) return;
  if (n > 9) return;
  _digitBuffer[digit] = convertNum(n);
}

// Display a character on a specific digit (limited set)
void sequencerDisplay::Char(byte digit, char c) {
  if (digit > 2) return;
  _digitBuffer[digit] = convertChar(c);
}

// Display scrolling text on the display. This function is blocking.
void sequencerDisplay::Text(String s) {
  // Pad with spaces so that scrolling begins and ends with blank displays.
  s = "   " + s + "   ";
  int len = s.length();
  
  for (int i = 0; i <= len - 3; i++) {
    // Load the 3-character window into the buffer.
    for (int j = 0; j < 3; j++) {
      char c = s.charAt(i + j);
      if (c >= '0' && c <= '9') {
        _digitBuffer[j] = convertNum(c - '0');
      } else {
        _digitBuffer[j] = convertChar(c);
      }
    }
    unsigned long startTime = millis();
    while(millis() - startTime < 500) {
      update();
    }
  }
}

// update() should be called repeatedly (e.g., in loop()) to multiplex the display.
void sequencerDisplay::update() {
  // Turn off all digits
  for (int i = 0; i < 3; i++) {
    digitalWrite(_digitPins[i], HIGH);
  }
  
  // Send segment data for the current digit to the 74HC595
  send_byte(_digitBuffer[currentDigit]);
  
  // Activate the current digit (active low)
  digitalWrite(_digitPins[currentDigit], LOW);
  
  // Move to the next digit for the next update call
  currentDigit = (currentDigit + 1) % 3;
  
  // Brief delay to allow the digit to appear (adjust as needed)
  delay(2);
}

// Set a raw segment pattern on a given digit.
void sequencerDisplay::setDigit(byte digit, byte pattern) {
  if (digit < 3) {
    _digitBuffer[digit] = pattern;
  }
}

// The startupAnimation() method runs a chaser effect followed by a flasher effect.
void sequencerDisplay::startupAnimation() {
  // --- Chaser Effect ---
  // Define patterns for individual segments.
  // Bit order: bit0 = A, bit1 = B, ... , bit6 = G, bit7 = DP.
  byte segPatterns[7] = {
    0xFE,  // Only segment A on
    0xFD,  // Only segment B on
    0xFB,  // Only segment C on
    0xF7,  // Only segment D on
    0xEF,  // Only segment E on
    0xDF,  // Only segment F on
    0xBF   // Only segment G on
  };

  // Run two cycles of the chaser effect
  for (int cycle = 0; cycle < 2; cycle++) {
    // For each digit (0: left, 1: middle, 2: right)
    for (int d = 0; d < 3; d++) {
      // For each segment (A to G)
      for (int s = 0; s < 7; s++) {
        // Clear all digits
        for (int i = 0; i < 3; i++) {
          setDigit(i, 0xFF);
        }
        // Set the current digit to show only the chosen segment.
        setDigit(d, segPatterns[s]);
        
        // Run multiplexing updates for ~100 ms
        unsigned long startTime = millis();
        while (millis() - startTime < 30) {
          update();
        }
      }
    }
  }
  
  // --- Flasher Effect ---
  // Flash all digits with "8" (all segments on) then blank.
  for (int i = 0; i < 5; i++) {
    // Display "8" (lookup table: '8' is 0x80)
    for (int d = 0; d < 3; d++) {
      setDigit(d, 0x80);
    }
    unsigned long startTime = millis();
    while (millis() - startTime < 100) {
      update();
    }
    // Blank the display
    for (int d = 0; d < 3; d++) {
      setDigit(d, 0xFF);
    }
    startTime = millis();
    while (millis() - startTime < 100) {
      update();
    }
  }
}

// Convert a number (0-9) to its 7-segment code.
byte sequencerDisplay::convertNum(byte n) {
  return numCodes[n];
}

// Convert a character (A-F, etc.) to a 7-segment code.
// Extend this function to support more characters if desired.
byte sequencerDisplay::convertChar(char c) {
  switch(toupper(c)) {
    case '0': return 0xC0;
    case '1': return 0xF9;
    case '2': return 0xA4;
    case '3': return 0xB0;
    case '4': return 0x99;
    case '5': return 0x92;
    case '6': return 0x82;
    case '7': return 0xF8;
    case '8': return 0x80;
    case '9': return 0x90;
    case 'A': return 0x88;  // A (0x77 inverted)
    case 'B': return 0x83;  // B (0x7C inverted)
    case 'C': return 0xC6;  // C (0x39 inverted)
    case 'D': return 0xA1;  // D (0x5E inverted)
    case 'E': return 0x86;  // E (0x79 inverted)
    case 'F': return 0x8E;  // F (0x71 inverted)
    case 'G': return 0xC2;  // G (0x3D inverted)
    case 'H': return 0x89;  // H (0x76 inverted)
    case 'I': return 0xCF;  // I (0x30 inverted)
    case 'J': return 0xE1;  // J (0x1E inverted)
    case 'K': return 0xFF;  // Not supported
    case 'L': return 0xC7;  // L (0x38 inverted)
    case 'M': return 0xFF;  // Not supported
    case 'N': return 0xAB;  // N (0x54 inverted)
    case 'O': return 0xC0;  // O (same as 0)
    case 'P': return 0x8C;  // P (0x73 inverted)
    case 'Q': return 0xFF;  // Not supported
    case 'R': return 0xAF;  // R (0x50 inverted)
    case 'S': return 0x92;  // S (same as 5)
    case 'T': return 0x87;  // T (0x78 inverted)
    case 'U': return 0xC1;  // U (0x3E inverted)
    case 'V': return 0xE3;  // V (0x1C inverted)
    case 'W': return 0xFF;  // Not supported
    case 'X': return 0xFF;  // Not supported
    case 'Y': return 0x91;  // Y (0x6E inverted)
    case 'Z': return 0xFF;  // Not supported
    case ' ': return 0xFF;  // Blank
    default:  return 0xFF;  // Unsupported characters are blank
  }
}

// send_byte() sends a byte to the 74HC595 via shiftOut.
void sequencerDisplay::send_byte(byte dataByte) {
  digitalWrite(_RCLK, LOW);
  shiftOut(_DIO, _SCLK, MSBFIRST, dataByte);
  digitalWrite(_RCLK, HIGH);
}

// activateDigit() turns off all digits then activates the specified one.
// (Not used in update(), but provided for completeness.)
void sequencerDisplay::activateDigit(byte digit) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(_digitPins[i], HIGH);
  }
  if (digit < 3) {
    digitalWrite(_digitPins[digit], LOW);
  }
}
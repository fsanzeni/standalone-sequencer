#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
#include "sequencerDisplay.h"

// Create an instance of the display:
// (Replace the pin numbers with your wiring configuration.)
// For example: data pin = 21, latch pin = 20, clock pin = 19,
// digit control pins = 7, 5, and 3.
sequencerDisplay myDisplay(21, 20, 19, 7, 5, 3);
Encoder inputEncoder(9, 10);

long position = -999;

void setup() {
  // Run the startup animation
  myDisplay.startupAnimation();

  // Set the encoder's initial position to 720 so that (720 / 4) equals 180.
  inputEncoder.write(180 * 4);  // 720
}

void loop() {
  long rawPos = inputEncoder.read();
  long detentPos = rawPos / 4;

  // Clamp the value so it never goes below 0.
  if (detentPos < 0) {
    detentPos = 0;
  }

  if (detentPos != position) {
    position = detentPos;
    
    // Convert the number to a 3-digit string.
    String posStr = String(position);
    while (posStr.length() < 3) {
      posStr = " " + posStr;  // Padding with spaces (or use "0" if you want zeros)
    }
    if (posStr.length() > 3) {
      // If the number is more than 3 digits, display the last three digits
      posStr = posStr.substring(posStr.length() - 3);
    }
    
    // Update each digit individually using Num() or Char() functions.
    for (int i = 0; i < 3; i++) {
      char c = posStr.charAt(i);
      if (c >= '0' && c <= '9') {
        myDisplay.Num(i, c - '0');
      } else {
        myDisplay.Char(i, c);
      }
    }
  }
  // Continuously refresh the display.
  myDisplay.update();
}
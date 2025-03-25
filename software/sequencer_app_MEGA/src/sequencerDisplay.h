#ifndef SEQUENCERDISPLAY_H
#define SEQUENCERDISPLAY_H

#include "Arduino.h"

class sequencerDisplay {
  public:
    // Constructor: DIO = data pin, RCLK = latch pin, SCLK = clock pin,
    // digit1, digit2, digit3 are the control pins for the three digits.
    sequencerDisplay(int DIO, int RCLK, int SCLK, int digit1, int digit2, int digit3);
    
    // Clear the display (all digits off)
    void Clear();
    
    // Display a number (0-9) on a specific digit (0, 1, or 2)
    void Num(byte digit, byte n);
    
    // Display a character (limited set) on a specific digit
    void Char(byte digit, char c);
    
    // Display text on all digits. If text is too long, it scrolls (blocking).
    void Text(String s);
    
    // Call update() repeatedly in loop() to maintain multiplexing.
    void update();
    
    // Set the raw segment pattern on a given digit (0 = segment on, 1 = off)
    void setDigit(byte digit, byte pattern);
    
    // Run a startup animation.
    void startupAnimation();

  private:
    // Pins for 74HC595 control:
    int _DIO, _RCLK, _SCLK;
    
    // Array for the three digit control pins (active low)
    int _digitPins[3];
    
    // Buffer for the segment data for each digit (0 = on, 1 = off for common anode)
    byte _digitBuffer[3];
    
    // Keeps track of which digit is currently being refreshed (for multiplexing)
    byte currentDigit;
    
    // Helper functions for converting numbers and characters
    byte convertNum(byte n);
    byte convertChar(char c);
    
    // Helper function to send a byte to the 74HC595
    void send_byte(byte dataByte);
    
    // (Optional) Activate a specific digit (turn on its common anode)
    void activateDigit(byte digit);
};

#endif
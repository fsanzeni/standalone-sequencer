#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h> // Ensure Arduino types and functions are available

class Display {
  private:
    // Pins for the shift register interface
    int _DIO;      // Data input pin
    int _RCLK;     // Register clock (latch) pin
    int _SCLK;     // Serial clock pin

    // Array holding the digit control pins (e.g., for multiplexing digits)
    int _digitPins[3];

    // Variables for number display and font data
    int _numDisplay;            // The number currently to be displayed
    uint8_t _digitDisplay[3];   // Extracted digits from the number
    uint8_t _alphaDisplay[3];   // Font data for alphanumeric display

    // Current digit index for multiplexing
    uint8_t _currentDigit;

    // Timing variable for multiplexing updates
    unsigned long _lastUpdate;

    // Private helper function to determine if the current digit should be visible
    bool currentDigitVisible();

  public:
    // Constructor
    Display();

    // Initialization: set up shift register and digit control pins
    // d1, d2, d3: Arduino pins controlling the three digit commons
    void init(int DIO, int RCLK, int SCLK, int d1, int d2, int d3);

    // Run a startup animation sequence on the display
    void startupSequence();

    // Set the number to be displayed (and update digit buffers)
    void setDisplayNum(int num);

    // Update the display; this should be called repeatedly (multiplexing)
    void update();

    // Set alphanumeric display value from a 3-character array (modifiable version)
    void setDisplayAlphaVar(char displayAlpha[3]);

    // Set alphanumeric display value from a constant 3-character array
    void setDisplayAlpha(const char displayAlpha[3]);

    // Adjust the brightness level of the display (range defined elsewhere)
    void setBrightness(int brightness);

    // Control the decimal point state (on/off)
    void setDecimal(bool decimalOn);

    // Blank (turn off) the current digit’s segments
    void blankSevenSegmentDisplay();

    // Move to the next digit for multiplexing
    void nextDigit();

    // Append or remove the decimal point in the display's font data
    void appendDecimal();

    // Send a byte of data to the shift register
    void sendByte(byte dataByte);

    void blinkDisplay(bool blinking, int interval, int timeout);

};

#endif

// Pin Definitions
const int dataPin = 21;   // SER (pin 14) of 74HC595
const int clockPin = 19;  // SRCLK (pin 11) of 74HC595
const int latchPin = 20;  // RCLK (pin 12) of 74HC595

const int digit1Pin = 16; // Common anode for digit 1
const int digit2Pin = 17; // Common anode for digit 2
const int digit3Pin = 18; // Common anode for digit 3

// Segment patterns for 0-9 (the first number controls the decimal point: 1 - OFF, 0 - ON)
byte sevenSegDigits[10] = {
  0b11000000, // 0
  0b11111001, // 1
  0b10100100, // 2
  0b10110000, // 3
  0b10011001, // 4
  0b10010010, // 5
  0b10000010, // 6
  0b11111000, // 7
  0b10000000, // 8
  0b10010000  // 9
};

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(digit1Pin, OUTPUT);
  pinMode(digit2Pin, OUTPUT);
  pinMode(digit3Pin, OUTPUT);
  
  // Initialise all digits off
  digitalWrite(digit1Pin, LOW);
  digitalWrite(digit2Pin, LOW);
  digitalWrite(digit3Pin, LOW);
}

void loop() {
  for (int num = 0; num < 10; num++) {
    unsigned long startTime = millis();
    while (millis() - startTime < 1000) { // Display each number for 1 second
      // Refresh all digits repeatedly
      for (int digit = 0; digit < 3; digit++) {
        // Turn off all digits
        digitalWrite(digit1Pin, LOW);
        digitalWrite(digit2Pin, LOW);
        digitalWrite(digit3Pin, LOW);

        // Send segment data
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, sevenSegDigits[num]);
        digitalWrite(latchPin, HIGH);

        // Activate current digit
        switch(digit) {
          case 0: digitalWrite(digit1Pin, HIGH); break;
          case 1: digitalWrite(digit2Pin, HIGH); break;
          case 2: digitalWrite(digit3Pin, HIGH); break;
        }
        
        delay(2); // Adjust for brightness/flicker
      }
    }
  }
}
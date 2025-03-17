#include <Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <elapsedMillis.h>

// Pin Definitions
const byte dataPin = 21;   // SER (pin 14) of 74HC595
const byte clockPin = 19;  // SRCLK (pin 11) of 74HC595
const byte latchPin = 20;  // RCLK (pin 12) of 74HC595

const byte digit1Pin = 16;  // Common anode for digit 1
const byte digit2Pin = 17;  // Common anode for digit 2
const byte digit3Pin = 18;  // Common anode for digit 3

byte currentDisplayDigits[3] = {10, 10, 10}; // Start with all off
byte currentDigit = 0;
unsigned long previousTime = 0;
const unsigned long refreshInterval = 2; // Milliseconds per digit

const byte LED_PIN = 6;  // LED pin, most are valid

// Define the number of rows and columns
const byte ROWS = 4;
const byte COLS = 8;

byte rowPins[ROWS] = { 46, 48, 50, 52 };                  // Connect to the row pinouts of the keypad
byte colPins[COLS] = { 39, 41, 43, 45, 47, 49, 51, 53 };  // Connect to the column pinouts of the keypad

// Define number of LEDs
const byte LED_COUNT = 17;

// Declare NeoPixel object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Define a keymap
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A', 'B', 'C', 'D', 'E' },
  { '4', '5', '6', 'F', 'G', 'H', 'I', 'J' },
  { '7', '8', '9', 'K', 'L', 'M', 'N', 'O' },
  { '*', '0', '#', 'P', 'Q', 'R', 'S', 'T' }
};

// Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Segment patterns for 0-9 (the first number controls the decimal point: 1 - OFF, 0 - ON)
byte sevenSegDigits[11] = {
  0b11000000,  // 0
  0b11111001,  // 1
  0b10100100,  // 2
  0b10110000,  // 3
  0b10011001,  // 4
  0b10010010,  // 5
  0b10000010,  // 6
  0b11111000,  // 7
  0b10000000,  // 8
  0b10010000,  // 9
  0b11111111   // off
};

void setup() {
  screenSetup();
  //LEDsSetup(50);  // Argument: Max Brightness, number between 0 and 255
  currentDisplayDigits[0] = 2; // Digit 1 = 0
  currentDisplayDigits[1] = 1; // Digit 2 = off
  currentDisplayDigits[2] = 5; // Digit 3 = off
}

void loop() {
  //readKeyPress();
  //updateDisplay(); // Update the display continuously
}

void screenSetup() {
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

void LEDsSetup(byte maxBrightness) {
  strip.begin();
  strip.show();
  strip.setBrightness(maxBrightness);
}

void updateDisplay() {
  if (millis() - previousTime >= refreshInterval) {
    previousTime = millis();

    // Turn off all digits
    digitalWrite(digit1Pin, LOW);
    digitalWrite(digit2Pin, LOW);
    digitalWrite(digit3Pin, LOW);

    // Send data for the CURRENT digit
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, sevenSegDigits[currentDisplayDigits[currentDigit]]); // Use MSB or LSB
    digitalWrite(latchPin, HIGH);

    // Activate the current digit
    switch (currentDigit) {
      case 0: digitalWrite(digit1Pin, HIGH); break;
      case 1: digitalWrite(digit2Pin, HIGH); break;
      case 2: digitalWrite(digit3Pin, HIGH); break;
    }

    currentDigit = (currentDigit + 1) % 3; // Cycle through digits
  }
}

void readKeyPress() {
  // Read the pressed key
  char key = keypad.getKey();

  if (key != NO_KEY) {  // A valid key press is detected
    if (key == '1') {
      // SHIFT

    } else if (key == '2') {
      // PLAY

    } else if (key == '3') {
      // LOAD

    } else if (key == 'A') {
      // SAVE

    } else if (key == '4') {
      // STEP 1
      currentDisplayDigits[0] = 0;
      currentDisplayDigits[1] = 0;
      currentDisplayDigits[2] = 1;
    } else if (key == '5') {
      // STEP 2
      byte newDigits[3] = {10, 10, 2};
      memcpy(currentDisplayDigits, newDigits, sizeof(newDigits));
    } else if (key == '6') {
      // STEP 3

    } else if (key == 'F') {
      // STEP 4

    } else if (key == 'G') {
      // STEP 5

    } else if (key == 'H') {
      // STEP 6

    } else if (key == 'I') {
      // STEP 7

    } else if (key == 'J') {
      // STEP 8

    } else if (key == '7') {
      // STEP 9

    } else if (key == '8') {
      // STEP 10

    } else if (key == '9') {
      // STEP 11

    } else if (key == 'K') {
      // STEP 12

    } else if (key == 'L') {
      // STEP 13

    } else if (key == 'M') {
      // STEP 14

    } else if (key == 'N') {
      // STEP 15

    } else if (key == 'O') {
      // STEP 16

    } else if (key == '*') {
      // RECORD

    } else if (key == '0') {
      // MUTATE

    } else if (key == '#') {
      // GLIDE
    } else {
      // ERROR OF SOME SORT
    }
  }
}

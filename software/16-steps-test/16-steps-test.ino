#include <Keypad.h>

// Define the number of rows and columns
const byte ROWS = 4;
const byte COLS = 8;

// Pin assignments for the rows and columns
byte rowPins[ROWS] = { 46, 48, 50, 52 };                  // Connect to the row pinouts of the keypad
byte colPins[COLS] = { 39, 41, 43, 45, 47, 49, 51, 53 };  // Connect to the column pinouts of the keypad

// Define a keymap
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A', 'B', 'C', 'D', 'E' },
  { '4', '5', '6', 'F', 'G', 'H', 'I', 'J' },
  { '7', '8', '9', 'K', 'L', 'M', 'N', 'O' },
  { '*', '0', '#', 'P', 'Q', 'R', 'S', 'T' }
};

// Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  Serial.println("Custom Macro Pad Initialized");
}

void loop() {
  // Read the pressed key
  char key = keypad.getKey();

  if (key != NO_KEY) {  // A valid key press is detected
    if (key == '1') {
      Serial.println("SHIFT");
    } else if (key == '2') {
      Serial.println("PLAY");
    } else if (key == '3') {
      Serial.println("LOAD");
    } else if (key == 'A') {
      Serial.println("SAVE");
    } else if (key == '4') {
      Serial.println("STEP 1");
    } else if (key == '5') {
      Serial.println("STEP 2");
    } else if (key == '6') {
      Serial.println("STEP 3");
    } else if (key == 'F') {
      Serial.println("STEP 4");
    } else if (key == 'G') {
      Serial.println("STEP 5");
    } else if (key == 'H') {
      Serial.println("STEP 6");
    } else if (key == 'I') {
      Serial.println("STEP 7");
    } else if (key == 'J') {
      Serial.println("STEP 8");
    } else if (key == '7') {
      Serial.println("STEP 9");
    } else if (key == '8') {
      Serial.println("STEP 10");
    } else if (key == '9') {
      Serial.println("STEP 11");
    } else if (key == 'K') {
      Serial.println("STEP 12");
    } else if (key == 'L') {
      Serial.println("STEP 13");
    } else if (key == 'M') {
      Serial.println("STEP 14");
    } else if (key == 'N') {
      Serial.println("STEP 15");
    } else if (key == 'O') {
      Serial.println("STEP 16");
    } else if (key == '*') {
      Serial.println("RECORD");
    } else if (key == '0') {
      Serial.println("MUTATE");
    } else if (key == '#') {
      Serial.println("GLIDE");
    } else {
      // Serial.print("Key pressed: ");
      // Serial.println(key);
    }
  }
}

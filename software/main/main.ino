/*
 * Audio Sequencer Keyboard Input - First Pass
 *
 * This sketch scans a button matrix arranged in 4 columns and 8 rows.
 * Out of the total buttons, the first 16 (IDs 0 to 15) are assumed to be
 * the sequencer steps. The remaining buttons can be used for other controls.
 *
 * Columns are driven low one at a time while the rows are read.
 *
 * Adjust the pin assignments and mapping logic as necessary for your hardware.
 */

const int NUM_COLS = 4;
const int NUM_ROWS = 8;

// Pin assignments for Arduino MEGA (adjust as needed)
int colPins[NUM_COLS] = {22, 23, 24, 25};  
int rowPins[NUM_ROWS] = {26, 27, 28, 29, 30, 31, 32, 33};

// State array to track debounced button states
bool buttonState[NUM_ROWS][NUM_COLS] = {false};

void setup() {
  Serial.begin(9600);
  
  // Set column pins as outputs and initialize them HIGH (inactive)
  for (int col = 0; col < NUM_COLS; col++) {
    pinMode(colPins[col], OUTPUT);
    digitalWrite(colPins[col], HIGH);
  }
  
  // Set row pins as inputs with internal pull-ups enabled.
  for (int row = 0; row < NUM_ROWS; row++) {
    pinMode(rowPins[row], INPUT_PULLUP);
  }
}

void loop() {
  scanMatrix();
  delayMicroseconds(50); // Basic delay for debouncing
}

void scanMatrix() {
  // For each column...
  for (int col = 0; col < NUM_COLS; col++) {
    // Activate this column by driving it LOW.
    digitalWrite(colPins[col], LOW);
    
    // Read each row in the active column.
    for (int row = 0; row < NUM_ROWS; row++) {
      bool pressed = (digitalRead(rowPins[row]) == LOW);
      
      // If the button has just been pressed...
      if (pressed && !buttonState[row][col]) {
        buttonState[row][col] = true;
        // Calculate a unique button ID (simple mapping: row * NUM_COLS + col)
        int buttonId = row * NUM_COLS + col;
        
        // Check if it's one of the 16 sequencer steps
        if (buttonId < 16) {
          Serial.print("Sequencer Step ");
          Serial.print(buttonId);
          Serial.println(" pressed.");
          // Insert code to toggle or trigger the sequencer step here.
        } else {
          Serial.print("Control Button ");
          Serial.print(buttonId);
          Serial.println(" pressed.");
          // Insert code to handle additional controls here.
        }
      }
      
      // If the button has been released...
      if (!pressed && buttonState[row][col]) {
        buttonState[row][col] = false;
        // Optionally add code to handle button release events.
      }
    }
    
    // Deactivate the current column by setting it HIGH again.
    digitalWrite(colPins[col], HIGH);
  }
}

// Arduino pin assignments for 74HC595
const int dataPin = 21;   // SER (pin 14) of 74HC595
const int clockPin = 19;  // SRCLK (pin 11) of 74HC595
const int latchPin = 20;  // RCLK (pin 12) of 74HC595

// Digit control pins (active low for common anode via mmbt3906)
// DIGIT1: pin 7, DIGIT2: pin 5, DIGIT3: pin 3
const int digitPins[3] = {7, 5, 3};

// Common anode segment codes for digits 0-9
// (These values are the bitwise inversions of the typical common cathode codes)
// Bit order: bit0 = A, bit1 = B, bit2 = C, bit3 = D, bit4 = E, bit5 = F, bit6 = G, bit7 = DP (unused)
byte digitCodes[10] = {
  0xC0, // 0: inverted 0x3F (segments A, B, C, D, E, F on)
  0xF9, // 1: inverted 0x06 (segments B, C on)
  0xA4, // 2: inverted 0x5B (segments A, B, D, E, G on)
  0xB0, // 3: inverted 0x4F (segments A, B, C, D, G on)
  0x99, // 4: inverted 0x66 (segments B, C, F, G on)
  0x92, // 5: inverted 0x6D (segments A, C, D, F, G on)
  0x82, // 6: inverted 0x7D (segments A, C, D, E, F, G on)
  0xF8, // 7: inverted 0x07 (segments A, B, C on)
  0x80, // 8: inverted 0x7F (all segments on)
  0x90  // 9: inverted 0x6F (segments A, B, C, D, F, G on)
};

unsigned long previousMillis = 0;
const unsigned long interval = 1000; // Update the count every 1 second
int count = 0;

void setup() {
  // Initialize digit control pins
  for (int i = 0; i < 3; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], HIGH); // Turn off all digits (active low)
  }
  
  // Initialize 74HC595 control pins
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  
  // Clear the shift register (all segments off: 0xFF means no LED lit)
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0xFF);
  digitalWrite(latchPin, HIGH);
}

void loop() {
  // Update the count every 1 second
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    count++;
    if(count >= 1000) {
      count = 0; // Roll over after 999
    }
  }
  
  // Extract hundreds, tens, and ones
  int hundreds = count / 100;
  int tens = (count / 10) % 10;
  int ones = count % 10;
  
  // Refresh each digit one at a time (multiplexing)
  displayDigit(0, hundreds); // Left digit (hundreds)
  displayDigit(1, tens);     // Middle digit (tens)
  displayDigit(2, ones);     // Right digit (ones)
}

// This function displays a single digit on one of the three displays
// digitIndex: 0 for hundreds, 1 for tens, 2 for ones
// number: value between 0 and 9 to display
void displayDigit(int digitIndex, int number) {
  // Turn off all digits
  for (int i = 0; i < 3; i++) {
    digitalWrite(digitPins[i], HIGH);
  }
  
  // Send the segment data for the given number to the 74HC595
  byte segData = digitCodes[number];
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, segData);
  digitalWrite(latchPin, HIGH);
  
  // Enable the selected digit (active low)
  digitalWrite(digitPins[digitIndex], LOW);
  delay(2); // Briefly display this digit (adjust delay as needed)
  digitalWrite(digitPins[digitIndex], HIGH); // Turn it off before next refresh
}
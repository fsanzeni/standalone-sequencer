#include <Arduino.h>
#include <elapsedMillis.h>
#include "display.h"
// #include "pinout.h"
#include "font.h"

bool decimal = 0;

// Move these variables into the class as private members
Display::Display() : 
  num_display(999),
  currentDigit(0),
  lastUpdate(0) {
  // Initialize digit arrays
  for(int i=0; i<3; i++) {
    digit_display[i] = 0;
    alpha_display[i] = 0;
  }
}

void Display::init(int DIO, int RCLK, int SCLK, int digit1, int digit2, int digit3) {
  // Store pin numbers in class members
  this->DIO = DIO;
  this->RCLK = RCLK;
  this->SCLK = SCLK;
  this->digitPins[0] = digit1;
  this->digitPins[1] = digit2;
  this->digitPins[2] = digit3;

  // Initialize pins
  pinMode(DIO, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SCLK, OUTPUT);
  
  for(int i=0; i<3; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], HIGH); // Turn off digits
  }
  setDisplayNum(0);
}

void Display::startupSequence() {
  byte segPatterns[7] = {
    0xFE,  // Only segment A on
    0xFD,  // Only segment B on
    0xFB,  // Only segment C on
    0xF7,  // Only segment D on
    0xEF,  // Only segment E on
    0xDF,  // Only segment F on
    0xBF   // Only segment G on
  };

  // Light each segment sequentially across all digits
  for (int i = 0; i < 7; i++) {
    sendByte(segPatterns[i]);
    
    // Activate all three digits simultaneously
    for (int d = 0; d < 3; d++) {
      digitalWrite(digitPins[d], LOW);
    }
    
    delay(100);  // Keep segments lit for 100ms
    
    // Turn off all digits
    for (int d = 0; d < 3; d++) {
      digitalWrite(digitPins[d], HIGH);
    }
    
    delay(50);  // Brief pause between segments
  }

  // Final flourish: light all segments briefly
  sendByte(0x00);  // All segments on
  for (int d = 0; d < 3; d++) {
    digitalWrite(digitPins[d], LOW);
  }
  delay(200);
  
  // Clean up: turn off all digits and segments
  for (int d = 0; d < 3; d++) {
    digitalWrite(digitPins[d], HIGH);
  }
  sendByte(0xFF);  // All segments off
}


void Display::setDisplayNum(int displayNum) {
	if (num_display == displayNum) return;
	num_display = displayNum;
	
	// Handle negative numbers properly
	if(num_display < 0) {
	  digit_display[0] = 37; // Minus sign
	  int absVal = abs(num_display);
	  digit_display[1] = (absVal / 10) % 10;
	  digit_display[2] = absVal % 10;
	  
	  // Handle single-digit negatives
	  if(absVal < 10) digit_display[1] = 36;
	} else {
	  digit_display[0] = (num_display / 100) % 10;
	  digit_display[1] = (num_display / 10) % 10;
	  digit_display[2] = num_display % 10;
	  
	  // Blank leading zeros
	  if(num_display < 100) digit_display[0] = 36;
	  if(num_display < 10) digit_display[1] = 36;
	}
	
	// Apply segment patterns
	alpha_display[0] = alphabet[digit_display[0]];
	alpha_display[1] = alphabet[digit_display[1]];
	alpha_display[2] = alphabet[digit_display[2]];
  appendDecimal();
}

void Display::update() {
  if(millis() - lastUpdate >= 2) { // Non-blocking 2ms delay
    lastUpdate = millis();

    // Turn off all digits first
    for(int i=0; i<3; i++) {
      digitalWrite(digitPins[i], HIGH);
    }

    // Send segment data with proper latch timing
    sendByte(alpha_display[currentDigit]);

    // Enable current digit (active low)
    digitalWrite(digitPins[currentDigit], LOW);

    // Move to next digit
    currentDigit = (currentDigit + 1) % 3;
  }
}

void Display::sendByte(byte dataByte) {
  digitalWrite(RCLK, LOW);
    shiftOut(DIO, SCLK, MSBFIRST, dataByte);
    digitalWrite(RCLK, HIGH);
}

void Display::appendDecimal(){
	if (decimal) {
		alpha_display[0] &= alphabet[38];
	} else {
		alpha_display[0] |= ~alphabet[38];
	}
}
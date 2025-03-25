#include <Arduino.h>
#include "pinout.h"
#include "display.h"

Display seqDisplay;

void setup() {
  seqDisplay.init(DIO_PIN, RCLK_PIN, SCLK_PIN, DIGIT_1_PIN, DIGIT_2_PIN, DIGIT_3_PIN);
  seqDisplay.startupSequence();
}

void loop() {
  seqDisplay.update();
  seqDisplay.setDisplayNum(888);

  // static unsigned long last = 0;
  //   if(millis() - last > 500) {
  //     last = millis();
  //     static int n = 0;
  //     seqDisplay.setDisplayNum(n++);
  //     if(n > 30) n = -99; // Test negative values
  //   }
  
}

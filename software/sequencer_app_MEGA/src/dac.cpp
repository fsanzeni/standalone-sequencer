#include "Pinout.h"   // Ensure that this header defines CSDAC_PIN, DAC_DATA_PIN, and DAC_CLOCK_PIN
#include "Dac.h"
#include <Arduino.h>

// Note: Since 'val' is unsigned, no need to check for negative values.
void Dac::setOutput(uint8_t channel, uint8_t gain, uint8_t shutdown, unsigned int val)
{
  // Clamp val to the 12-bit maximum.
  if (val > 4095) {
    val = 4095;
  }
  
  // Build the 12-bit data into two bytes.
  // The low byte contains the lower 8 bits.
  uint8_t lowByte = val & 0xFF;
  
  // The high byte gets the upper 4 bits of the value.
  // Then we OR in the control bits:
  // - 'channel' shifted left by 7
  // - 'gain' shifted left by 5
  // - 'shutdown' shifted left by 4.
  uint8_t highByte = ((val >> 8) & 0x0F) | (channel << 7) | (gain << 5) | (shutdown << 4);

  // Begin transmission to the DAC:
  // Pull the DAC chip-select pin low.
  digitalWrite(CSDAC_PIN, LOW);

  // Use shiftOut to send the high and low bytes sequentially.
  // The data is sent with MSBFIRST.
  shiftOut(DAC_DATA_PIN, DAC_CLOCK_PIN, MSBFIRST, highByte);
  shiftOut(DAC_DATA_PIN, DAC_CLOCK_PIN, MSBFIRST, lowByte);

  // Pull the chip-select pin high to end transmission.
  digitalWrite(CSDAC_PIN, HIGH);
}

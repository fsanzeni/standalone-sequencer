#pragma once

// MISC PINS
#define GATE_PIN                9
//#define LDAC_PIN 1
#define CLOCK_OUT_PIN           8
#define CLOCK_IN_PIN            11
#define RESET_PIN               10

// KEYBOARD PINS
#define ROW0_PIN                46
#define ROW1_PIN                48
#define ROW2_PIN                50
#define ROW3_PIN                52

#define COL0_PIN                39
#define COL1_PIN                41
#define COL2_PIN                43
#define COL3_PIN                45
#define COL4_PIN                47
#define COL5_PIN                49
#define COL6_PIN                51
#define COL7_PIN                53

const int buttonRows[4] = { ROW0_PIN, ROW1_PIN, ROW2_PIN, ROW3_PIN };
const int buttonCols[8] = { COL0_PIN, COL1_PIN, COL2_PIN, COL3_PIN,
                            COL4_PIN, COL5_PIN, COL6_PIN, COL7_PIN };

// LED PINS
#define GLIDE_NEOPIXEL_PIN      23
#define LED_MATRIX_PIN          2

// TODO
// #define GLIDE_LED_PIN 15 //these 8 are on the button driver chip, not the MCU
#define GLIDE_PIN               14
#define REPEAT_PIN              13
#define RECORD_PIN              12
#define SAVE_PIN                11 
#define LOAD_PIN                10 
#define PLAY_PIN                9 
#define SHIFT_PIN               8

// TODO - CHIP SELECT PINS
// #define CS0_PIN  10 //MCP23S17 for buttons, aux LEDS
// #define CS1_PIN  9  //Display/matrix shift registers
#define CSFLASH_PIN             8  //Flash memory


// DIGIT DISPLAY PINS
#define DIGIT_1_PIN             16
#define DIGIT_2_PIN             17
#define DIGIT_3_PIN             18
#define DIO_PIN                 21
#define RCLK_PIN                20
#define SCLK_PIN                19

// DAC PINS
#define CSDAC_PIN               7
#define DAC_DATA_PIN            5
#define DAC_CLOCK_PIN           6

// POTENTIOMETER PINS - TODO
#define ANALOG_PIN_1            A0
#define ANALOG_PIN_2            A1
#define ANALOG_PIN_3            A2
#define ANALOG_PIN_4            A3

// ENCODER PINS
#define ENC_A_PIN               A4
#define ENC_B_PIN               A5
#define ENC_PORT                PINC

// GAIN
#define GAIN_1                  0x1
#define GAIN_2                  0x0

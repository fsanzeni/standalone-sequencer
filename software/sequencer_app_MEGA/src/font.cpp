#include "font.h"

const uint8_t alphabet[40]= {
    0xC0, // [0] => "0"
    0xF9, // [1] => "1"
    0xA4, // [2] => "2"
    0xB0, // [3] => "3"
    0x99, // [4] => "4"
    0x92, // [5] => "5"
    0x82, // [6] => "6"
    0xF8, // [7] => "7"
    0x80, // [8] => "8"
    0x90, // [9] => "9"
    0x88, // [10] => "A"
    0x83, // [11] => "b"
    0xC6, // [12] => "C"
    0xA1, // [13] => "d"
    0x86, // [14] => "E"
    0x8E, // [15] => "F"
    0xC2, // [16] => "g" (9)
    0x89, // [17] => "h"
    0xCF, // [18] => "i"
    0xE1, // [19] => "J"
    0xFF, // [20] => "K"        //do not use
    0xC7, // [21] => "L"
    0xAB, // [22] => "M"
    0xAB, // [23] => "n"
    0xC0, // [24] => "o"
    0x8C, // [25] => "P"
    0xFF, // [26] => "q"        //do not use
    0xAF, // [27] => "r"
    0x92, // [28] => "S" (5)
    0x87, // [29] => "t"
    0xC1, // [30] => "u"
    0xE3, // [31] => "V"
    0xFF, // [32] => "W" (V)    //do not use
    0xFF, // [33] => "X" (H)    //do not use
    0x91, // [34] => "Y"    
    0xFF, // [35] => "Z"        //do not use
    0xFF, // [36] => blank
    0xBF, // [37] => -
    0xFE  // [38] => .
};
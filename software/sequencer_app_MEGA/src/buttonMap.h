#ifndef BUTTON_MAP_H
#define BUTTON_MAP_H

// Matrix layout (indices shown are physical positions 0..31):
//
//  Row 0 (indices 0 - 7):
//    [0]  SHIFT      --> Logical ID  0  
//    [1]  PLAY       --> Logical ID  1  
//    [2]  LOAD       --> Logical ID  2  
//    [3]  SAVE       --> Logical ID  3  
//    [4-7] Unused
//
//  Row 1 (indices 8 - 15): Step buttons 1-8
//    [8]   Step 1     --> Logical ID 10
//    [9]   Step 2     --> Logical ID 11
//    [10]  Step 3     --> Logical ID 12
//    [11]  Step 4     --> Logical ID 13
//    [12]  Step 5     --> Logical ID 14
//    [13]  Step 6     --> Logical ID 15
//    [14]  Step 7     --> Logical ID 16
//    [15]  Step 8     --> Logical ID 17
//
//  Row 2 (indices 16 - 23): Step buttons 9-16 with alternate functions when SHIFT is pressed
//    [16]  Step 9     --> Normal Logical ID: 18
//                      Alternate: TEMPO      (Logical ID 30)
//    [17]  Step 10    --> Normal Logical ID: 19
//                      Alternate: STEPS      (Logical ID 31)
//    [18]  Step 11    --> Normal Logical ID: 20
//                      Alternate: SCALE      (Logical ID 32)
//    [19]  Step 12    --> Normal Logical ID: 21
//                      Alternate: SWING      (Logical ID 33)
//    [20]  Step 13    --> Normal Logical ID: 22
//                      Alternate: TRANSPOSE  (Logical ID 34)
//    [21]  Step 14    --> Normal Logical ID: 23
//                      Alternate: CALIBRATE  (Logical ID 35)
//    [22]  Step 15    --> Normal Logical ID: 24
//                      Alternate: CLEAR      (Logical ID 36)
//    [23]  Step 16    --> Normal Logical ID: 25
//                      (No alternate specified)
//
//  Row 3 (indices 24 - 31):
//    [24]  RECORD     --> Logical ID 26
//    [25]  MUTATE     --> Logical ID 27
//                      Alternate: TYPE        (Logical ID 37)
//    [26]  GLIDE      --> Logical ID 28
//                      Alternate: LENGTH      (Logical ID 38)
//    [27-31] Unused
//
// In this mapping, unused positions are set to -1.

const int button_map[32] = {
    // Row 0:
     0,  // index 0: SHIFT
     1,  // index 1: PLAY
     2,  // index 2: LOAD
     3,  // index 3: SAVE
    -1,  // index 4: Unused
    -1,  // index 5: Unused
    -1,  // index 6: Unused
    -1,  // index 7: Unused

    // Row 1 (Step buttons 1-8):
    10, // index 8:  Step 1
    11, // index 9:  Step 2
    12, // index 10: Step 3
    13, // index 11: Step 4
    14, // index 12: Step 5
    15, // index 13: Step 6
    16, // index 14: Step 7
    17, // index 15: Step 8

    // Row 2 (Step buttons 9-16 with alternates):
    18, // index 16: Step 9   (Alternate: TEMPO  -> 30)
    19, // index 17: Step 10  (Alternate: STEPS  -> 31)
    20, // index 18: Step 11  (Alternate: SCALE  -> 32)
    21, // index 19: Step 12  (Alternate: SWING  -> 33)
    22, // index 20: Step 13  (Alternate: TRANSPOSE -> 34)
    23, // index 21: Step 14  (Alternate: CALIBRATE -> 35)
    24, // index 22: Step 15  (Alternate: CLEAR  -> 36)
    25, // index 23: Step 16

    // Row 3:
    26, // index 24: RECORD
    27, // index 25: MUTATE   (Alternate: TYPE   -> 37)
    28, // index 26: GLIDE    (Alternate: GLIDE LENGTH -> 38)
    -1, // index 27: Unused
    -1, // index 28: Unused
    -1, // index 29: Unused
    -1, // index 30: Unused
    -1  // index 31: Unused
};

// Alternate function mapping for row 2 (only used when SHIFT is pressed)
// Each entry corresponds to the physical button in row 2 (indices 16 to 23).
// For buttons that have an alternate function, we map to a different logical ID.
const int button_map_alternate[9] = {
    30, // for physical index 16 (Step 9): TEMPO
    31, // for physical index 17 (Step 10): STEPS
    32, // for physical index 18 (Step 11): SCALE
    33, // for physical index 19 (Step 12): SWING
    34, // for physical index 20 (Step 13): TRANSPOSE
    35, // for physical index 21 (Step 14): CALIBRATE
    36, // for physical index 22 (Step 15): CLEAR
    37, // for physical index 27 (MUTATE): TYPE
    38, // for physical index 28 (GLIDE): GLIDE LENGTH
};

#endif // BUTTON_MAP_H
#pragma once

#include "display.h"
#include "sequencer.h"

class LedMatrix{
    public:
        void init(Display &display, Sequencer &sequencer);

        void updateMatrix(int row);

        void blankMatrix(int row);

        void multiplexLeds();

        void blinkStep(); 
        
        void selectStep(int step);

        void blinkLed();

        void blinkCurrentStep();

        void reset();

        void setMatrixFromSequencer(byte bar);

        void setMatrix(bool *matrix);

        void toggleLed(int led);
    
};
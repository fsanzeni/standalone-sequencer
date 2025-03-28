#pragma once

class Encoder{
    public:
        void init();
        int poll();
        int smartPoll();
        int encoder_amount = 0;
        int increment_amount = 0;
        bool toggle_inverted();
    private:
        void encoder_increment(int amt);
};
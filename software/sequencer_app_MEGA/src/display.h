class Display {
    private:
      int DIO, RCLK, SCLK;
      int digitPins[3];
      int num_display;
      uint8_t digit_display[3];
      uint8_t alpha_display[3];
      uint8_t currentDigit;
      unsigned long lastUpdate;
    
    public:
      Display();
      void init(int DIO, int RCLK, int SCLK, int d1, int d2, int d3);
      void startupSequence();
      void setDisplayNum(int num);
      void update();
      void appendDecimal();
      void sendByte(byte dataByte);
    };
#include "SequencerKeyboard.h"

SequencerKeyboard seqKey;

void setup() {
  Serial.begin(9600);
  seqKey.begin();
}

void loop() {
  SequencerKeyboard::KeyEvent ev = seqKey.getKeyEvent();
  if(ev.code != SequencerKeyboard::KEY_NONE) {
    switch(ev.code) {
      case SequencerKeyboard::KEY_SHIFT:
        Serial.println("SHIFT pressed");
        break;
      case SequencerKeyboard::KEY_PLAY:
        Serial.println("PLAY");
        break;
      case SequencerKeyboard::KEY_LOAD:
        Serial.println("LOAD");
        break;
      case SequencerKeyboard::KEY_SAVE:
        Serial.println("SAVE");
        break;
      case SequencerKeyboard::KEY_RECORD:
        Serial.println("RECORD");
        break;
      case SequencerKeyboard::KEY_MUTATE:
        Serial.println("MUTATE");
        break;
      case SequencerKeyboard::KEY_TYPE:
        Serial.println("TYPE (alternate for MUTATE)");
        break;
      case SequencerKeyboard::KEY_GLIDE:
        Serial.println("GLIDE");
        break;
      case SequencerKeyboard::KEY_GLIDE_LENGTH:
        Serial.println("GLIDE LENGTH (alternate for GLIDE)");
        break;
      case SequencerKeyboard::KEY_STEP:
        Serial.print("STEP ");
        Serial.println(ev.step);
        break;
      case SequencerKeyboard::KEY_TEMPO:
        Serial.println("TEMPO (alternate for step 9)");
        break;
      // ... handle other alternate functions similarly ...
      default:
        break;
    }
  }
  
  // Example reading potentiometers:
  int pitchVal = seqKey.readPitch();
  // Do something with pitchVal...
}

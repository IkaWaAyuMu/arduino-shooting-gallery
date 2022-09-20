#include <IRremote.hpp>
#include "DEFINE.h"

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(RECIEVER_PIN, false);
}

void loop() {
  if(IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.resume();
  }
}

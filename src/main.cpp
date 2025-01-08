#include <Arduino.h>
#include "Config.h"
#include "SimModule/SIM7600.h"
#include "PowerSupply/PwModule.h"

SIM7600 simModule(Serial1);
PwModule pwModule;
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  pwModule.powerModule();
  pwModule.powerKey();
  simModule.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
}
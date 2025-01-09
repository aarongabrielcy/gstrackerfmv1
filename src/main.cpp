#include <Arduino.h>
#include "Config.h"
#include "SimModule/SIM7600.h"
#include "PowerSupply/PwModule.h"
#include "Network/NetManager.h"

SIM7600 simModule(Serial1);
PwModule pwModule;
NetManager netManager(simModule);

bool fix = 0;

void handleSerialInput();

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  pwModule.powerModule();
  pwModule.powerKey();
  simModule.begin();
  netManager.activeTcpService();
  pwModule.powerLedGnss();
}

void loop() {
  handleSerialInput();
  pwModule.blinkLedGnss(fix);
}
void handleSerialInput() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    String response = simModule.sendCommandWithResponse(command.c_str(), AT_COMMAND_TIMEOUT);
    Serial.print("CLEAN RSP |=> ");
    Serial.println(response);
  }
  
}
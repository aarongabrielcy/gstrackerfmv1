#ifndef SIM7600_H
#define SIM7600_H

#include <Arduino.h>
#include "Config.h"
#include "Utils.h"
#include "Littles_enums.h"
class SIM7600 {

public:
  SIM7600(HardwareSerial& serial);
  void begin();
  String sendCommandWithResponse(const char* command, int timeout);
  String processResponse(const String& command, const String& fcommand, const String& response);

private:
  HardwareSerial& simSerial;
  bool isAllowedCommand(const String& command);
  int commandType(const String& command);

  static const char* allowedCommands[35];  // Declaración de miembro estático
};

#endif
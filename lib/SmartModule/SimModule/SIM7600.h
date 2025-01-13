#ifndef SIM7600_H
#define SIM7600_H

#include <Arduino.h>
#include "Config.h"
#include "Utils.h"
#include "Littles_enums.h"
class SIM7600 {
private:
  HardwareSerial& simSerial;
  bool isAllowedCommand(const String& command);
  int commandType(const String& command);

  static const char* allowedCommands[37];
public:
  SIM7600(HardwareSerial& serial);
  void begin();
  String sendCommandWithResponse(const char* command, int timeout);
  String processResponse(const String& command, const String& fcommand, const String& response);
  String sendReadDataToServer(const String& fcommand, const String& message, int timeout);
  String sendReadDataToGNSS(int timeout);
  Utils utils;
};

#endif
#include "Information.h"

Information::Information(SIM7600& sim7600) : simModule(sim7600) {}

String Information::getIMEI() {
    String imei_cmd = "AT+SIMEI?";
    String imei = simModule.sendCommandWithResponse(imei_cmd.c_str(), 4000);
    return imei;
}
String Information::getDevID(){
    String imei_cmd = "AT+SIMEI?";
    String imei = simModule.sendCommandWithResponse(imei_cmd.c_str(), 4000);
    return formatDevID(imei);
}
String Information::formatDevID(String input) {
  // Verifica que el string tenga al menos 10 caracteres
  if (input.length() >= 10) {
    // Devuelve los últimos 10 caracteres
    return input.substring(input.length() - 10);
  } else {
    // Si el string es menor que 10 caracteres, devuelve el string completo
    return input;
  }
}
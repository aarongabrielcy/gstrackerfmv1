#include "SIM7600.h"

const char* SIM7600::allowedCommands[57] = {
    "AT","D0","D1", "D2", "F", "W", "CTZR", "CFUN", "NETCLOSE", "CGDCONT","CGDSCONT","COPS", "CSQ", "NETOPEN", "CIPOPEN", "CIPSEND", "CIPCLOSE", "CIPMODE","CGATT", "CGACT",
    "SIMEI", "CCID", "CPSI", "CNMP", "CNSMOD",  "CCLK", "CTZU", "CGPS", "CGPSINFO", "CGPSCOLD", "CGPSHOT", "CGNSSINFO", "CREG", "CGPADDR","CGPSNMEA","CMEE","CPIN", "CSPN", 
    "CSCLK", "CBC", "CPMUTEMP", "CNAOP", "CGREG", "CMGF", "CPMS", "CNMI", "CMGS", "CMGR", "CMGL", "CMGD", "CSCA", "CSMS", "CSCB", "CSMP", "CSDH", "CMGRD", "CMGSEX"
};
const char* SIM7600::internalCommands[5] = {gstCmds::RESET, gstCmds::SECO_ON, gstCmds::SECO_OFF, gstCmds::CONFIG, gstCmds::APN_IP };
// Constructor
SIM7600::SIM7600(HardwareSerial& serial) : simSerial(serial) {}

void SIM7600::begin() {
  simSerial.begin(SIM7600_BAUD_RATE, SERIAL_8N1, SIM7600_RX_PIN, SIM7600_TX_PIN);
}

String SIM7600::sendCommandWithResponse(const char* command, int timeout) {
  /*Serial.print("Enviando comando => ");
  Serial.println(command);*/
  int type = commandType(command);

  String formattedCommand = String(command).substring(3); // Quitar "AT+"
  if(type == READ){
    //quitar cualquier sufijo "?"
   // Serial.println("Quitando sufijo... ");
    if (formattedCommand.endsWith("?") ) {
      formattedCommand.remove(formattedCommand.length() - 1); // Remover sufijo
    }
  }else if(type == WRITE){
    //Serial.println("quitando signo '='");
    // Buscar si hay un '=' en el comando, y eliminar todo lo que esté después
    int posEqual = formattedCommand.indexOf("=");
    if (posEqual != -1) {
      formattedCommand = formattedCommand.substring(0, posEqual);  // Mantener solo hasta antes del '='
    }
  }else if(type == TEST) {
    if (formattedCommand.endsWith("=?") ) {
      formattedCommand.remove(formattedCommand.length() - 2); // Remover sufijo
    }
  }else if(type == CMD) {
    formattedCommand.replace(";","");
    String get_imei = formattedCommand.substring(0, 10);
    String get_inter_cmd = formattedCommand.substring(10);
    Serial.println("get IMEI: "+ get_imei);
    Serial.println("get Command:"+ get_inter_cmd);
    if(get_imei == imei) {
        String res =  isInternalCommands(get_inter_cmd);
        return String(Headers::RES)+DLM+command+DLM+res;
    }
    
  }
  /*Serial.print("Comando formateado: ");
  Serial.println(formattedCommand);*/
  
  if (!isAllowedCommand(formattedCommand)) {  
    Serial.println("Comando no permitido.");
    return "INVALID COMMAND";
  }
  if(formattedCommand == "AT") {
      simSerial.println(formattedCommand);  // Enviar comando
  }else{
    simSerial.println(command);  // Enviar comando    
  }
  String response = "";
  long startTime = millis();
  while ((millis() - startTime) < timeout) {
    if (simSerial.available()) {
      char c = simSerial.read();
      response += c;
    }
  }

  /*Serial.println("Respuesta completa: ");
  Serial.println(response);}*/
  return processResponse(command, formattedCommand, response);
}
String SIM7600::processResponse(const String& command,  const String& fcommand, const String& response) {
  String state_command = "";
  String processedResponse = response;
  processedResponse.replace(String(command), "");
  //processedResponse.replace("AT+" + fcommand, "");
  processedResponse.replace("+" + fcommand + ": ", "");
  /*Serial.print("Procesando respuesta... ");
  Serial.println(processedResponse);*/

  processedResponse = utils.trimResponse(processedResponse);
  
  if (processedResponse.endsWith("OK")) {
    processedResponse.remove(processedResponse.length() - 2);
    state_command = "OK";
    //Serial.println("Estado del comando: "+state_command);
  } else if (processedResponse.endsWith("ERROR")) {
    processedResponse.remove(processedResponse.length() - 5);
    state_command = "ERROR";
    //Serial.println("Estado del comando: "+state_command);
    return "ERROR COMMAND";
  }

  /*Serial.print("Respuesta procesada: ");
  Serial.println(processedResponse);*/
  if(processedResponse.length() == 0){
    return state_command;
  }
  return processedResponse;
}

String SIM7600::isInternalCommands(const String& command) {
  Serial.println("conmand ?> "+command );
  if (command == String(gstCmds::RESET)) {
    Serial.println("Reiniciando modulo por comando!");
    return "Reboot";
  }else if(command == String(gstCmds::CONFIG)) {
    Serial.println("Consultando configuracion del modulo!");
    return "INFO";
  }else if(command == String(gstCmds::APN_IP)) {
    Serial.println("configuracion del APN Ejecutado!");
    return "TRUE";
  }
  else {
    return "Unknown CMD";
  }
  return "ERR";
}

bool SIM7600::isAllowedCommand(const String& command) {
  for (const char* cmd : allowedCommands) {
    if (command == cmd) {
      return true;
    }
  }
  return false;
}

int SIM7600::commandType(const String& command) {
  if (command.endsWith("=?")){
    //Serial.println("Es un comando de prueba (TEST).");
    return TEST;
  }else if (command.endsWith("?") && command.indexOf('=') == -1){
    //Serial.println("Es un comando de lectura (READ).");
    return READ;
  }else if (command.indexOf('=') != -1 && !command.endsWith("?")){ 
    //Serial.println("Es un comando de escritura (WRITE).");
    return WRITE;
  }else if (command.startsWith("AT+") && command.indexOf('=') == -1){
   //Serial.println("Es un comando de ejecución (EXECUTE).");
   return EXECUTE;
  }else if(command.startsWith("STT") ) {  
    Serial.println("ES una Cadena de texto (SEND).");
    return SEND;
  }else if(command.startsWith("CMD") ) {  
    Serial.println("ES un COMANDO interno.");
    return CMD;
  }
  else { 
    //Serial.println("Tipo de comando desconocido.");
    return UNKNOWN;
  }
}
String SIM7600::sendReadDataToServer(const String& fcommand, const String& message, int timeout) {

  simSerial.println(message);  // Enviar comando  
  String response = "";
  long startTime = millis();
  while ((millis() - startTime) < timeout) {
    if (simSerial.available()) {
      char c = simSerial.read();
      response += c;
    }
  }
  
  response.replace(message, "");
  /*response.replace("+"+fcommand+": ", "");*/
  response = utils.trimResponse(response);
  
  return response; //devolver un JSON con el formato [{server: ""}, {port:0},{length:0},{response: ""}]
}
String SIM7600::readDataToGNSS(int timeout) {
  String rawdata = "";
  
  long startTime = millis();
  while ((millis() - startTime) < timeout) {
    if (simSerial.available()) {
      char c = simSerial.read();
      rawdata += c;
    }
  }
  return utils.cleanGnssData(rawdata);
}
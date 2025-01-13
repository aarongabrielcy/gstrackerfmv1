#include "ReadToSendData.h"

ReadToSendData::ReadToSendData(SIM7600& sim7600) : simModule(sim7600) {}

bool ReadToSendData::sendData(String message, int timeout) {
    Serial.print("DATA => ");
  Serial.println(message);
  String respServer = "";

  String cmd = "AT+CIPSEND=0," + String(message.length());
  String response = simModule.sendCommandWithResponse(cmd.c_str(), timeout);
  
  if (response.indexOf(">") != -1) {
    String respServer = simModule.sendReadDataToServer("CIPSEND", message, timeout); // Envía el mensaje   
    Serial.println("Respuesta procesada => " + respServer);
    String respCMD = readData(respServer, timeout);
    Serial.println("Comando recibido =>"+respCMD);
  }else {
    Serial.println("Error al enviar mensaje TCP. Intentando reconexión...");
    return false;
  }
    return true;
}

String ReadToSendData::readData(String data, int timeout) {
  int startIndex = data.indexOf("CMD");
  if (startIndex == -1) {
    // Si no encuentra "CMD", devuelve una cadena vacía
    return "";
  }
  if(data.substring(startIndex) == "CMD;02049830910;04;01"){
    Serial.println("Activa salida ===========>");
    
  }else if(data.substring(startIndex) == "CMD;02049830910;04;02"){
  
    Serial.println("Desactiva salida ===========>");
  }

  // Extrae desde "CMD" hasta el final de la cadena
  return data.substring(startIndex);  
}
#include "NetManager.h"

NetManager::NetManager(SIM7600& sim7600) : simModule(sim7600) {}

bool NetManager::initializeModule(){
  String at_cmd = "AT+AT";
  String at = simModule.sendCommandWithResponse(at_cmd.c_str(), 4000);
  if(at == "AT"){
    return true;
  }
  Serial.println("Error initializing module!");
  return false;
}

void NetManager::activeTcpService() {
    String netactive_cmd = "AT+NETOPEN";
    String netactive = simModule.sendCommandWithResponse(netactive_cmd.c_str(), 3000);
    Serial.println("Active TCP => " + netactive);
}

void NetManager::configTcpServer(const String& server, int port) {
    String cip_cmd = "AT+CIPOPEN=0,\"TCP\",\"" + server + "\"," +port;
     String cip = simModule.sendCommandWithResponse(cip_cmd.c_str(), 3000);
}
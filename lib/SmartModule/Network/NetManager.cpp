#include "NetManager.h"

NetManager::NetManager(SIM7600& sim7600) : simModule(sim7600) {}

void NetManager::activeTcpService() {
    String netactive_cmd = "AT+NETOPEN";
    String netactive = simModule.sendCommandWithResponse(netactive_cmd.c_str(), 4000);
    Serial.println("Active TCP => " + netactive);
}

bool NetManager::configTcpServer(const String& server, int port) {
    return false;
}
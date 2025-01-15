#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <Arduino.h>
#include "SimModule/SIM7600.h"
#include "Utils.h"
class NetManager {
    private:
        Utils utils;
        SIM7600& simModule;
    public:
        NetManager(SIM7600& sim7600);
        void activeTcpService();
        void configTcpServer(const String& server, int port);
        bool initializeModule();
        String getDateTime();
        bool tcpServiceStatus();
};
#endif

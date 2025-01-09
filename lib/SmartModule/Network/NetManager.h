#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <Arduino.h>
#include "SimModule/SIM7600.h"

class NetManager {
    private:
        SIM7600& simModule;
    public:
        NetManager(SIM7600& sim7600);
        void activeTcpService();
        bool configTcpServer(const String& server, int port);
};
#endif

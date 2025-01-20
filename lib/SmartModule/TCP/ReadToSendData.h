#ifndef READTOSENDDATA_H
#define READTOSENDDATA_H

#include <Arduino.h>
#include "SimModule/SIM7600.h"
#include "Utils.h"

class ReadToSendData {
private:
    SIM7600& simModule;
    Utils utils;
public:
    ReadToSendData(SIM7600& SIM7600);
    bool sendData(String message, int timeout);
    String readData(String data, int timeout);
};

#endif
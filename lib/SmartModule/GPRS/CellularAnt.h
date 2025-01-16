#ifndef CELLULARANT_H
#define CELLULARANT_H

#include <Arduino.h>
#include <vector>
#include "SimModule/SIM7600.h"

class CellularAnt {
private:
    SIM7600& simModule;
    std::vector<String> splitString(const char *data, char delimiter);
public:
    CellularAnt(SIM7600& SIM7600);
    String cellInformation(int timeout);
    struct CellularData {
        String systemMode;
        String operationMode;
        String mcc;
        String mnc;
        String lac;
        String cellId;
        String frequencyBand;
        int psc;
        int freq;
        int ssc;
        float ecIO;
        int rscp;
        int qual;  
        int rxLev;
        int txPwr;
    };

    CellularData parse(const char *data);
};
#endif
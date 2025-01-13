#ifndef INFORMATION_H
#define INFORMATION_H
#include <Arduino.h>
#include "SIM7600.h"

class Information {

private:
    SIM7600& simModule;
    String formatDevID(String input);
public:
    Information(SIM7600& sim7600);
    String getIMEI();
    String getDevID();
};

#endif
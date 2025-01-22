#ifndef SMSMANAGER_H
#define SMSMANAGER_H

#include <Arduino.h>
#include "SimModule/SIM7600.h"

class SmsManager
{
private:
    SIM7600& simModule;
public:
    SmsManager(SIM7600& sim7600);

};

#endif
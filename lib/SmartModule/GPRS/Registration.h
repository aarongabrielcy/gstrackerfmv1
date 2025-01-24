#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <Arduino.h>
#include "SimModule/SIM7600.h"
class Registration {
private:
    SIM7600& simModule;
public:
    Registration(SIM7600& sim7600);
    void apnRegistration(char *apn);
    bool netRegistrationState();
    bool networkRegistration();
    void softReset();
};

#endif
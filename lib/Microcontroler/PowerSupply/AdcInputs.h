#ifndef ADCINPUTS_H
#define ADCINPUTS_H

#include <Arduino.h>
#include "Config.h"

class AdcInputs {
private:
    const int R1_divisor_power_in = 3300; //we read the voltage acrosst this resistor (car resistors)
    const int R2_divisor_power_in = 100000;
    const int R1_divisor_batt = 5100; //we read the voltage acrosst this resistor (backup resistors)
    const int R2_divisor_batt = 1800;
public:
    float getPowerValue();
    float getBattValue();
};

#endif
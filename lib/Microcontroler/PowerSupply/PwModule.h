#ifndef PWMODULE_H
#define PWMODULE_H

#include <Arduino.h>
#include "Config.h"

class PwModule {
private:
    unsigned long previousMillis = 0;
    unsigned long ledInterval = 400;
    int ledState = LOW, pinIgn;
public:
    void powerKey();
    void powerModule();
    void powerLedGnss();
    void blinkLedGnss(int fixState);
    void initInIgn(int pin);
    bool getStateIgn();
    void OffModule();
    void restartMicro();
};
#endif

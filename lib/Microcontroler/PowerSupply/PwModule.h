#ifndef CONNECTION_H
#define CONNECTION_H

#include <Arduino.h>
#include "Config.h"

class PwModule {
public:
    void powerKey();
    void powerModule();
    void powerLedGnss();
    void blinkLedGnss(int fixState);

private:
    unsigned long previousMillis = 0;
    unsigned long ledInterval = 150; // Ejemplo: intervalo de parpadeo de 1 segundo
    int ledState = LOW; // Estado inicial del LED
};
#endif

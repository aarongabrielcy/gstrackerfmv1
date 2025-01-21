#include "PwModule.h"

void PwModule::powerModule(){
  pinMode(POWER_SIM_PIN, OUTPUT);
  digitalWrite(POWER_SIM_PIN, HIGH);  
}
void PwModule::OffModule(){
  pinMode(POWER_SIM_PIN, OUTPUT);
  digitalWrite(POWER_SIM_PIN, LOW);  
}

void PwModule::restartMicro() {
  esp_restart();
}

void PwModule::powerKey(){
  pinMode(POWER_KEY_PIN, OUTPUT);
  digitalWrite(POWER_KEY_PIN, LOW); 
  delay(1000);
  digitalWrite(POWER_KEY_PIN, HIGH);
  delay(3000);
}

void PwModule::powerLedGnss(){
  pinMode(GNSS_LED_PIN, OUTPUT);
  digitalWrite(GNSS_LED_PIN, false);
}
void PwModule::blinkLedGnss(int fixState) {
  unsigned long currentMillis = millis();
   if (currentMillis - previousMillis >= ledInterval) {
        previousMillis = currentMillis; // Actualiza el tiempo del último cambio

        // Cambia el estado del LED
      fixState == 0? ledState = !ledState : ledState = fixState;
      digitalWrite(GNSS_LED_PIN, ledState);
   }
}
void PwModule::initInIgn(int ignPin){
  this->pinIgn = ignPin;
  pinMode(ignPin, INPUT);
}

bool PwModule::getStateIgn(){
  return digitalRead(pinIgn);
}


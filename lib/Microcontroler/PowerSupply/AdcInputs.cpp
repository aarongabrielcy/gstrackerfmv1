#include "AdcInputs.h"

float AdcInputs::getPowerValue(){
  uint32_t voltage_mV2 = analogReadMilliVolts(ADC_IN_POWER); // Read the voltage in millivolts
  int mV2 = voltage_mV2;
  float pwr = (((float) voltage_mV2) / 1000.0)  * (1 + (float)R2_divisor_power_in/(float)R1_divisor_power_in);
  return pwr;
}
float AdcInputs::getBattValue(){
  uint32_t voltage_mV1 = analogReadMilliVolts(ADC_IN_BATTERY);
  int mv1 = voltage_mV1;
  float batt = (((float) voltage_mV1) / 1000.0)  * (1 + (float)R2_divisor_batt/(float)R1_divisor_batt);
  return batt;
}
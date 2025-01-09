#include <Arduino.h>
#include "Config.h"
#include "Utils.h"
#include "SimModule/SIM7600.h"
#include "PowerSupply/PwModule.h"
#include "Network/NetManager.h"
#include "GNSS/GpsManager.h"
#include "TCP/ReadToSendData.h"


SIM7600 simModule(Serial1);
PwModule pwModule;
NetManager netManager(simModule);
GpsManager gpsManager(simModule);
ReadToSendData readToSendData(simModule);
Utils utils;

unsigned long lastPrintTime = 0; // Tiempo del último envío
unsigned long interval = 20000;
float previousCourse = -1.0;
int trackingCourse = 0;

bool fix;
String GNSSData;
bool ignState;
String message;
String datetime; 
String latitude;
String longitude;

void handleSerialInput();
bool checkSignificantCourseChange(float currentCourse);
void setup() {

  Serial.begin(SERIAL_BAUD_RATE);
  pwModule.powerModule();
  pwModule.powerKey();
  pwModule.powerLedGnss();
  simModule.begin();
  do { Serial.println("Inicializando Modulo..."); } while(!netManager.initializeModule());
  netManager.activeTcpService();
  gpsManager.activeGps(1);
  gpsManager.confiGpsReports(1);
  netManager.configTcpServer(DEFAULT_SVR, DEFAULT_PORT);

}

void loop() {
  handleSerialInput();
  GNSSData = simModule.sendReadDataToGNSS(1000);
  GNSSData == ",,,,,,,,,,,,,,," ? fix = 0 : fix = 1;   
  GpsManager::GPSData gpsParseData = gpsManager.parse(GNSSData.c_str());
  pwModule.getStateIgn() ? ignState = 0 : ignState = 1;
  datetime = gpsParseData.date+";"+gpsParseData.utc_time;
  latitude = utils.formatCoordinates(gpsParseData.latitude, gpsParseData.ns_indicator);
  longitude = utils.formatCoordinates(gpsParseData.longitude, gpsParseData.ew_indicator);
  message = "STT;2049830928;3FFFFF;32;1.0.0;1;"+datetime+";103682809;334;020;40C6;20;"+latitude+";"+longitude+";" +String(gpsParseData.speed)+ ";" +
            String(gpsParseData.course) + ";" +String(gpsParseData.gps_svs)+";"+fix+";"+trackingCourse+"000000"+ignState+";00000000;1;1;0929;4.1;14.19";
  
  if (checkSignificantCourseChange(gpsParseData.course) && ignState == 1) {
    Serial.print(" Envío en curva = >");
    readToSendData.sendData(message, 1000);
    return; 
  }

  unsigned long currentTime = millis();
  if (currentTime - lastPrintTime >= interval && ignState == 1) {
    lastPrintTime = currentTime;
    Serial.println("DATA =>"+message);
    Serial.println("RAWDATA => " +GNSSData );
  }
  pwModule.blinkLedGnss(fix);
}
void handleSerialInput() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    String response = simModule.sendCommandWithResponse(command.c_str(), AT_COMMAND_TIMEOUT);
    Serial.print("CLEAN RSP |=> ");
    Serial.println(response);
  }
}
bool checkSignificantCourseChange(float currentCourse) {
  if (isnan(currentCourse)) {
    Serial.println("Advertencia: el valor del curso no es válido.");
    return false;
  }

  float difference = abs(currentCourse - previousCourse);
  if (difference >= 15) {
    Serial.print("Cambio significativo detectado en course: ");
    Serial.println(difference);
    trackingCourse = 1;
    previousCourse = currentCourse;  // Actualizar el valor anterior
    return true;
  }
  trackingCourse = 0;
  //previousCourse = currentCourse;  // Actualizar de todos modos para la próxima comparación
  return false;
}
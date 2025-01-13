#include <Arduino.h>
#include "Config.h"
#include "Utils.h"
#include "Littles_enums.h"
#include "SimModule/SIM7600.h"
#include "PowerSupply/PwModule.h"
#include "Network/NetManager.h"
#include "GNSS/GpsManager.h"
#include "TCP/ReadToSendData.h"
#include "SimModule/Information.h"
//#include "PowerSupply/AdcInputs.h"

SIM7600 simModule(Serial1);
PwModule pwModule;
NetManager netManager(simModule);
GpsManager gpsManager(simModule);
ReadToSendData readToSendData(simModule);
Information information(simModule);
//AdcInputs adcInputs;
Utils utils;

unsigned long lastPrintTime = 0; // Tiempo del último envío
unsigned long interval = 20000;
float previousCourse = -1.0;
int trackingCourse = 0;
bool LaststateIgnition = HIGH;

unsigned long previous_time_send = 0;
unsigned long previous_time_ign_off = 0;
const unsigned long sendDataTimeout = SEND_DATA_HEART_BEAT;
const unsigned long sendDataIgnOff = SEND_DATA_ING_OFF;

bool fix;
String GNSSData;
bool ignState;
String message;
String heart_beat;
String imei;
String datetime; 
String latitude;
String longitude;
String last_valid_latitude;
String last_valid_longitude;

void handleSerialInput();
bool checkSignificantCourseChange(float currentCourse);
void ignition_event(GpsManager::GPSData gpsData);
void event_generated(GpsManager::GPSData gpsData, int event);
void reconectServer();

void setup() {

  Serial.begin(SERIAL_BAUD_RATE);
  pwModule.powerModule();
  pwModule.powerKey();
  pwModule.powerLedGnss();
  simModule.begin();
  do { Serial.println("Inicializando Modulo..."); } while(!netManager.initializeModule());
  imei = information.getDevID();
  netManager.activeTcpService();
  gpsManager.activeGps(1);
  gpsManager.confiGpsReports(1);
  netManager.configTcpServer(DEFAULT_SVR, DEFAULT_PORT);
  pwModule.initInIgn(INPUT_IGN);
}

void loop() {
  handleSerialInput();
  GNSSData = simModule.sendReadDataToGNSS(1000);
  GNSSData == ",,,,,,,,,,,,,,," ? fix = 0 : fix = 1;   
  GpsManager::GPSData gpsParseData = gpsManager.parse(GNSSData.c_str());
  pwModule.getStateIgn() ? ignState = 0 : ignState = 1;
  ignition_event(gpsParseData);
  if(!fix ){
    datetime = netManager.getDateTime(); // "AT+CTZU=1" Actualiza la hora,"AT+CTZR=1" actualiza la zona horaria,"AT&W" guarda los datos en la memoria no volatil
      latitude = last_valid_latitude;
      longitude = last_valid_longitude;
  }else{
    datetime = gpsParseData.date+DLM+gpsParseData.utc_time;
    latitude = utils.formatCoordinates(gpsParseData.latitude, gpsParseData.ns_indicator);
    longitude = utils.formatCoordinates(gpsParseData.longitude, gpsParseData.ew_indicator);
    last_valid_latitude = latitude;
    last_valid_longitude = longitude;
  }
  /*float battery = adcInputs.getBattValue(); 
  float power = adcInputs.getPowerValue();*/
  
  message = String(Headers::STT)+DLM+imei+DLM+"3FFFFF;32;1.0.0;1;"+datetime+";103682809;334;020;40C6;20;"+latitude+DLM+longitude+DLM+gpsParseData.speed+DLM+
            gpsParseData.course+DLM+gpsParseData.gps_svs+DLM+fix+DLM+trackingCourse+"000000"+ignState+";00000000;1;1;0929"+DLM+"4.1;14.19";
  heart_beat = String(Headers::ALV)+DLM+imei;
  
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
  Serial.println("SATELLITES => "+ gpsParseData.gps_svs);
    readToSendData.sendData(message, 1000);
  }
  unsigned long current_time = millis();
  if(current_time - previous_time_send >= sendDataTimeout && ignState == 0) {

    previous_time_send = current_time;  // Actualizar el tiempo anterior
    Serial.println("tiempo transcurrido ACTIVAR HEART BEAT => "); 
    if(!readToSendData.sendData(heart_beat, 1000)) {
      Serial.println("HEART BEAT NO ENVIADO");
      void reconectServer();
    }
  }
  if(current_time - previous_time_ign_off >= sendDataIgnOff && ignState == 0) {
      
    previous_time_ign_off = current_time;  // Actualizar el tiempo anterior
    Serial.println("tiempo transcurrido con motor apagado => "); 
    if(!readToSendData.sendData(message, 1000)) {
      Serial.println("Mensaje con el motor apagado no enviado");
      void reconectServer();
    }
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
  if (difference >= CHANGE_COURSE) {
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
void ignition_event(GpsManager::GPSData gpsData) {
  int StateIgnition = pwModule.getStateIgn();
  if (StateIgnition == LOW && LaststateIgnition == HIGH) {
    Serial.println("*** ¡ignition ON! **** ");

    event_generated(gpsData, IGNITON_ON);
    
  }else if(StateIgnition == HIGH && LaststateIgnition == LOW) {
    Serial.println("**** ¡ignition OFF! ***** ");

    event_generated(gpsData, IGNITON_OFF);
  }
  LaststateIgnition = StateIgnition;
}
void event_generated(GpsManager::GPSData gpsData, int event) {

  String data_event = "";
    data_event = String(Headers::ALT)+DLM+imei+DLM+"3FFFFF;32;1.0.0;1;"+datetime+";103682809;334;020;40C6;20;"+latitude+DLM+longitude+DLM+gpsData.speed+DLM+
                 gpsData.course+DLM+gpsData.gps_svs+DLM+fix+DLM+trackingCourse+"000000"+ignState+";00000000;"+event+";;";
    Serial.println("Event => "+ data_event);
    readToSendData.sendData(data_event, 2000);
}

void reconectServer() {
  netManager.activeTcpService();
  netManager.configTcpServer(DEFAULT_SVR, DEFAULT_PORT);
}
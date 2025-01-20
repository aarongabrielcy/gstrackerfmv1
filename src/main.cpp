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
#include "PowerSupply/AdcInputs.h"
#include "GPRS/Registration.h"
#include "GPRS/CellularAnt.h"

SIM7600 simModule(Serial1);
PwModule pwModule;
NetManager netManager(simModule);
GpsManager gpsManager(simModule);
ReadToSendData readToSendData(simModule);
Information information(simModule);
Registration registration(simModule);
CellularAnt cellularAnt(simModule);
AdcInputs adcInputs;
Utils utils;

unsigned long lastPrintTime = 0; // Tiempo del último envío
unsigned long interval = 20000;
float previousCourse = -1.0;
int trackingCourse = 0;
bool LaststateIgnition = HIGH;

unsigned long previous_time_send = 0;
unsigned long previous_time_ign_off = 0;
const unsigned long sendDataTimeout = 1788000;
const unsigned long sendDataIgnOff = 1800000;

bool fix;
String GNSSData;
String CellData;

bool ignState;
bool gpsState;
String message;
String heart_beat;
String imei;
String datetime; 
String latitude = "0.0", longitude = "0.0"; // Coordenadas como String
String last_valid_latitude = "0.0", last_valid_longitude = "0.0"; // Últimas coordenadas válidas como String
double distanceAccumulated = 0.0; // Distancia acumulada
int reconectCounter = 0;
void handleSerialInput();
double calculateHaversine(double lat1, double lon1, double lat2, double lon2);
bool checkSignificantCourseChange(float currentCourse);
void ignition_event(CellularAnt::CellularData cellData, GpsManager::GPSData gpsData);
void event_generated(CellularAnt::CellularData cellData, GpsManager::GPSData gpsData, int event);
void reconectServices();

void setup() {

  Serial.begin(SERIAL_BAUD_RATE);
  pwModule.powerModule();
  pwModule.powerKey();
  pwModule.powerLedGnss();
  simModule.begin();

  do { Serial.println("Inicializando Modulo..."); gpsManager.confiGpsReports(0); } while(!netManager.initializeModule());
  registration.networkRegistration();
  do { Serial.println("conectado a la red local ..."); } while (!registration.netRegistrationState());
  do { Serial.println("conectado a la red celular ...");} while (!registration.networkRegistration());
  
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
  CellData = cellularAnt.cellInformation(1500);
  fix = GNSSData != ",,,,,,,,,,,,,,,";
  
  GpsManager::GPSData gpsParseData = gpsManager.parse(GNSSData.c_str());
  CellularAnt::CellularData cellParseData = cellularAnt.parse(CellData.c_str());

  pwModule.getStateIgn() ? ignState = 0 : ignState = 1;
  
  
  if (fix) {
    // Calcular la distancia desde la última posición válida
    double currentDistance = calculateHaversine(
      atof(last_valid_latitude.c_str()), atof(last_valid_longitude.c_str()), 
      gpsParseData.latitude, gpsParseData.longitude );
        // Acumular la distancia recorrida
      distanceAccumulated += currentDistance;

        // Verificar si se alcanzaron 100 metros
      if (distanceAccumulated >= 150.0 && ignState == 0 && gpsParseData.speed > 20) {
        Serial.println("Distancia recorrida >= 100m. Enviando datos...");
        readToSendData.sendData(message, 1000);
        distanceAccumulated = 0.0; // Reiniciar la distancia acumulada
      }
      datetime = gpsParseData.date+DLM+gpsParseData.utc_time;
      // Formatear las coordenadas para enviarlas en el mensaje
      latitude = utils.formatCoordinates(gpsParseData.latitude, gpsParseData.ns_indicator);
      longitude = utils.formatCoordinates(gpsParseData.longitude, gpsParseData.ew_indicator);
      // Actualizar las últimas coordenadas válidas en formato String
      last_valid_latitude = latitude;
      last_valid_longitude = longitude;

    } else {
        if(!gpsManager.stateGps() ) {
          Serial.println("gps apagado encendiendo... ");
          gpsManager.activeGps(1);
        }
        
        datetime = netManager.getDateTime(); // "AT+CTZU=1" Actualiza la hora,"AT+CTZR=1" actualiza la zona horaria,"AT&W" guarda los datos en la memoria no volatil
        // Usar las últimas coordenadas válidas si no hay fix
        latitude = last_valid_latitude;
        longitude = last_valid_longitude;
  }
  ignition_event(cellParseData, gpsParseData);
  float battery = adcInputs.getBattValue(); 
  float power = adcInputs.getPowerValue();
  
  if(gpsParseData.speed <= 3 && fix) {
    message = String(Headers::STT)+DLM+imei+DLM+"3FFFFF;32;1.0.0;1;"+datetime+DLM+cellParseData.cellId+DLM+cellParseData.mcc+DLM+cellParseData.mnc+
            DLM+cellParseData.lac+DLM+cellParseData.rxLev+DLM+last_valid_latitude+DLM+last_valid_longitude+DLM+gpsParseData.speed+DLM+gpsParseData.course+DLM+
            gpsParseData.gps_svs+DLM+fix+DLM+trackingCourse+"000000"+ignState+";00000000;1;1;0929"+DLM+battery+DLM+power;
  }else {
    message = String(Headers::STT)+DLM+imei+DLM+"3FFFFF;32;1.0.0;1;"+datetime+DLM+cellParseData.cellId+DLM+cellParseData.mcc+DLM+cellParseData.mnc+
            DLM+cellParseData.lac+DLM+cellParseData.rxLev+DLM+latitude+DLM+longitude+DLM+gpsParseData.speed+DLM+gpsParseData.course+DLM+
            gpsParseData.gps_svs+DLM+fix+DLM+trackingCourse+"000000"+ignState+";00000000;1;1;0929"+DLM+battery+DLM+power;
  }
  heart_beat = String(Headers::ALV)+DLM+imei;
  
  if (checkSignificantCourseChange(gpsParseData.course) && ignState == 1) {
    Serial.print(" Envío en curva = >");
    if(!readToSendData.sendData(message, 1000)) {
      Serial.println("TRACKEO POR CURVA NO ENVIADO");
      reconectServices();
    }
    return; 
  }

  unsigned long currentTime = millis();
  if (currentTime - lastPrintTime >= interval && ignState == 1) {
    lastPrintTime = currentTime;
    Serial.println("DATA =>"+message);
    Serial.println("RAWDATA => " +CellData );
    if(!readToSendData.sendData(message, 1000)) {
      //OMITIR EN LA FUNCION sendResposeCommand "+CGNSSINFO: ,,,,,,,,,,,,,,,"
      Serial.println("TRACKEO POR TIEMPO NO ENVIADO");
      reconectServices();
    }
  }
  unsigned long current_time = millis();
  if(current_time - previous_time_send >= sendDataTimeout && ignState == 0) {

    previous_time_send = current_time;  // Actualizar el tiempo anterior
    Serial.println("tiempo transcurrido ACTIVAR HEART BEAT => "); 
    if(!readToSendData.sendData(heart_beat, 1000)) {
      Serial.println("HEART BEAT NO ENVIADO");
      reconectServices();
    }
  }
  if(current_time - previous_time_ign_off >= sendDataIgnOff && ignState == 0) {
      
    previous_time_ign_off = current_time;  // Actualizar el tiempo anterior
    Serial.println("tiempo transcurrido con motor apagado => "); 
    if(!readToSendData.sendData(message, 1000)) {
      Serial.println("Mensaje con el motor apagado no enviado");
      reconectServices();
    }
  }
  pwModule.blinkLedGnss(fix);
}
void handleSerialInput() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    String response = simModule.sendCommandWithResponse(command.c_str(), AT_COMMAND_TIMEOUT);
    Serial.print("CLEAN RSP |=> ");
    Serial.println(utils.cleanDelimiter(response, "+CGNSSINFO: "));
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
void ignition_event(CellularAnt::CellularData cellData, GpsManager::GPSData gpsData) {
  int StateIgnition = pwModule.getStateIgn();
  if (StateIgnition == LOW && LaststateIgnition == HIGH) {
    Serial.println("*** ¡ignition ON! **** ");

    event_generated(cellData, gpsData, IGNITON_ON);
    
  }else if(StateIgnition == HIGH && LaststateIgnition == LOW) {
    Serial.println("**** ¡ignition OFF! ***** ");

    event_generated(cellData, gpsData, IGNITON_OFF);
  }
  LaststateIgnition = StateIgnition;
}
void event_generated(CellularAnt::CellularData cellData, GpsManager::GPSData gpsData, int event) {

  String data_event = "";
  data_event = String(Headers::ALT)+DLM+imei+DLM+"3FFFFF;32;1.0.0;1;"+datetime+DLM+cellData.cellId+DLM+cellData.mcc+
                DLM+cellData.mnc+DLM+cellData.lac+DLM+cellData.rxLev+DLM+latitude+DLM+longitude+DLM+gpsData.speed+
                DLM+gpsData.course+DLM+gpsData.gps_svs+DLM+fix+DLM+trackingCourse+"000000"+ignState+";00000000;"+event+";;";
    Serial.println("Event => "+ data_event);
    readToSendData.sendData(data_event, 2000);
}
double calculateHaversine(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371000; // Radio de la Tierra en metros
    double dLat = radians(lat2 - lat1);
    double dLon = radians(lon2 - lon1);
    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(radians(lat1)) * cos(radians(lat2)) * sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c; // Distancia en metros
}
void reconectServices() {
  //cuando reinicies el modulo activa el gps y gnssinfo de nuevo
  Serial.println("Reconectando Servicios.. ");
  registration.networkRegistration();
  netManager.activeTcpService();
  netManager.configTcpServer(DEFAULT_SVR, DEFAULT_PORT);
  reconectCounter++;

  if (reconectCounter == 10) {
    Serial.println("La función reconectServices() se ha ejecutado 10 veces.");
    registration.softReset();
    reconectCounter = 0; // Reiniciar el contador después de alcanzar 10
  }
}
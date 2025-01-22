#include "GpsManager.h"

GpsManager::GpsManager(SIM7600& sim7600) : simModule(sim7600) {}

bool GpsManager::stateGps() {
    String state_cmd = "AT+CGPS?";
    String state = simModule.sendCommandWithResponse(state_cmd.c_str(), 3000);
    state = utils.cleanDelimiter(state, "+CGNSSINFO: ");
    //Serial.println(state_cmd+" =>"+state+"*");
    if(state == "1,1OK") {
        //Serial.println(state_cmd+"if:true"+ state);
        return true;
    }else if( state == "1,1") {
        //Serial.println(state_cmd+"else:true"+ state);
        return true;
    }
    else if(state == "0,1OK") {
        Serial.println(state_cmd+"=>RSP: "+ state);
        return false;
    }
    else if(state == "0,1") {
        Serial.println(state_cmd+"=>RSP: "+ state);
        return false;
    }
    //Serial.println(state_cmd+"false"+ state);
    return true;
}
void GpsManager::activeGps(int state) {
    String cgps_cmd = "AT+CGPS="+String(state);
    String cgps = simModule.sendCommandWithResponse(cgps_cmd.c_str(), 3000);
      Serial.println(cgps_cmd+" => "+ cgps);
}
void GpsManager::confiGpsReports(int interval) {
  String cgnss_cmd = "AT+CGNSSINFO="+String(interval);
  String cgnss = simModule.sendCommandWithResponse(cgnss_cmd.c_str(), 3000);  
  Serial.println(cgnss_cmd+" => "+ cgnss);
}


// Método privado para dividir cadenas
std::vector<String> GpsManager::splitString(const char *data, char delimiter) {
    std::vector<String> tokens;
    String token;
    while (*data) {
        if (*data == delimiter) {
            tokens.push_back(token);
            token = "";
        } else {
            token += *data;
        }
        ++data;
    }
    if (!token.isEmpty()) {
        tokens.push_back(token);
    }
    return tokens;
}

// Método público para parsear datos GPS
GpsManager::GPSData GpsManager::parse(const char *data) {
    GPSData gpsData;
    auto tokens = splitString(data, ',');

    if (tokens.size() >= 16) {
        gpsData.mode = tokens[0].toInt();
        gpsData.gps_svs = tokens[1].toInt();
        gpsData.glonass_svs = tokens[2].toInt();
        gpsData.beidou_svs = tokens[3].toInt();
        //gpsData.latitude = tokens[4].toDouble();
        gpsData.latitude = !tokens[4].isEmpty() ? tokens[4].toDouble(): 0.00;
        gpsData.ns_indicator = tokens[5][0];
        //gpsData.longitude = tokens[6].toDouble();
        gpsData.longitude = !tokens[6].isEmpty() ? tokens[6].toDouble(): 0.00;
        gpsData.ew_indicator = tokens[7][0];
        gpsData.date = utils.formatDate(tokens[8]);
        gpsData.utc_time = utils.formatTime(tokens[9]);
        gpsData.altitude = tokens[10].toFloat();
       //gpsData.speed = tokens[11].toFloat();
        if (!tokens[11].isEmpty()) {
            gpsData.speed = tokens[11].toFloat() * 1.85;
        }
        gpsData.course = !tokens[12].isEmpty() ? tokens[12].toFloat(): 0.1;
        /*if (!tokens[12].isEmpty()) {
        float parsedCourse = tokens[12].toFloat();
        gpsData.course = (parsedCourse > 0) ? lastValidCourse = parsedCourse : lastValidCourse;
        } else {
        gpsData.course = lastValidCourse; // Usa el último valor válido si el campo está vacío
        }*/
        gpsData.pdop = tokens[13].toFloat();
        gpsData.hdop = tokens[14].toFloat();
        gpsData.vdop = tokens[15].toFloat();

    }
    return gpsData;
}

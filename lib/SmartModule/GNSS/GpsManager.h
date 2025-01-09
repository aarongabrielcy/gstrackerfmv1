#ifndef GPSMANAGER_H
#define GPSMANAGER_H

#include <Arduino.h>
#include "SimModule/SIM7600.h"
#include <vector>
#include "Utils.h"

class GpsManager {
    private:
        SIM7600& simModule;
        std::vector<String> splitString(const char *data, char delimiter);
        Utils utils;

    public:
        GpsManager(SIM7600& SIM7600);
        void activeGps(int state);
        void confiGpsReports(int interval);
        struct GPSData {
        int mode;
        int gps_svs;
        int glonass_svs;
        int beidou_svs;
        double latitude;
        char ns_indicator;
        double longitude;
        char ew_indicator;
        String date;
        String utc_time;
        float altitude;
        float speed;
        float course;
        float pdop;
        float hdop;
        float vdop;
    };
    GPSData parse(const char *data);
};
#endif
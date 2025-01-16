#ifndef GPSMANAGER_H
#define GPSMANAGER_H

#include <Arduino.h>
#include "SimModule/SIM7600.h"
#include <vector>
#include "Utils.h"
#include <string>

class GpsManager {
    private:
        SIM7600& simModule;
        std::vector<String> splitString(const char *data, char delimiter);
        Utils utils;
        float lastValidCourse = 0.0;

    public:
        GpsManager(SIM7600& SIM7600);
        void activeGps(int state);
        void confiGpsReports(int interval);
        struct GPSData {
        int mode;
        int gps_svs = 0;
        int glonass_svs;
        int beidou_svs;
        double latitude = 0.00;
        char ns_indicator;
        double longitude = 0.00;
        char ew_indicator;
        String date;
        String utc_time;
        float altitude;
        float speed = 0.00;
        float course = 0.00;
        float pdop;
        float hdop;
        float vdop;
    };
    GPSData parse(const char *data);
};
#endif
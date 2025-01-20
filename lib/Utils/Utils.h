#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "Config.h"
#include "Littles_enums.h"

class Utils
{
private:
    String getPositionData(String data,int position);
public:
    String trimResponse(const String& response);
    String cleanGnssData(const String& cleanData);
    String formatCoordinates(double coord, char direction);
    String formatDate(const String &date);
    String formatTime(const String &utcTime);
    String getFormatUTC(String dt);
    String cleanDelimiter(String str, const String &substring);
};
#endif
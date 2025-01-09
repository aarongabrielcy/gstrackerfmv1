#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

class Utils
{
private:

public:
    String trimResponse(const String& response);
    String cleanGnssData(const String& cleanData);
    String formatCoordinates(double coord, char direction);
    String formatDate(const String &date);
    String formatTime(const String &utcTime);
};
#endif
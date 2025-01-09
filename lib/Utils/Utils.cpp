#include "Utils.h"

String Utils::trimResponse(const String& response) {
  String result = response;      // Hacemos una copia modificable de response
  result.replace("\n", "");      // Elimina saltos de línea
  result.replace("\r", "");      // Elimina retornos de carro
  result.trim();                 // Elimina espacios en blanco al inicio y al final
  return result;
}

String Utils::cleanGnssData(const String& cleanData) {
  String processData = cleanData;
  processData = trimResponse(processData);
  processData.replace("+CGNSSINFO: ", "");
  return processData;
}

String Utils::formatCoordinates(double coord, char direction) {
    // Separar grados y minutos usando operaciones matemáticas
    int degrees = static_cast<int>(coord / 100); // Obtener los grados (parte entera)
    double minutes = coord - (degrees * 100);    // Obtener los minutos (parte decimal)
    double decimalDegrees = degrees + (minutes / 60.0); // Convertir a grados decimales

    // Aplicar signo dependiendo de la dirección
    if (direction == 'S' || direction == 'W') {
        decimalDegrees = -decimalDegrees;
    }

    // Formatear el resultado como string con signo explícito
    char buffer[12]; // Suficiente para contener el resultado con precisión
    snprintf(buffer, sizeof(buffer), "%+.6f", decimalDegrees); // "%+" añade el signo explícito para valores positivos
    return String(buffer);
}

String Utils::formatDate(const String &date) {
    // Formato de entrada: DDMMYY (e.g., 141124)
    // Formato de salida: YYYYMMDD (e.g., 20241121)
    String year = "20" + date.substring(4, 6); // Asume que el año es 20XX
    String month = date.substring(2, 4);
    String day = date.substring(0, 2);
    return year + month + day;
}

String Utils::formatTime(const String &utcTime) {
    // Formato de entrada: HHMMSS.s (e.g., 040641.0)
    // Formato de salida: HH:MM:SS (e.g., 04:06:41)
    String hours = utcTime.substring(0, 2);
    String minutes = utcTime.substring(2, 4);
    String seconds = utcTime.substring(4, 6);
    return hours + ":" + minutes + ":" + seconds;
}
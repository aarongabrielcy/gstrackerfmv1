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

String Utils::getFormatUTC(String dt) {
  String date = getPositionData(dt ,DATE);
    date.replace("\"", "");
    String timeWithTZ = getPositionData(dt, TIME);
    timeWithTZ.replace("\"", "");
    //Serial.println("dateTime UTC-6 => "+ date +";"+timeWithTZ);
     // Extraer fecha (formato recibido: "yy/MM/dd")
    int year = 2000 + date.substring(0, 2).toInt(); // Convertir "yy" en "yyyy"
    int month = date.substring(3, 5).toInt();
    int day = date.substring(6, 8).toInt();

    // Extraer hora (formato recibido: "hh:mm:ss±zz")
    int hour = timeWithTZ.substring(0, 2).toInt();
    int minute = timeWithTZ.substring(3, 5).toInt();
    int second = timeWithTZ.substring(6, 8).toInt();

    // Extraer y convertir zona horaria (en cuartos de hora)
    int timezoneOffset = timeWithTZ.substring(9).toInt() * 15; // ±zz en minutos
    int offsetHours = timezoneOffset / 60;
    int offsetMinutes = timezoneOffset % 60;

    // Ajuste a UTC sumando la diferencia horaria
    hour += offsetHours;
    minute += offsetMinutes;
    
    // Ajuste de minutos y horas si se supera el límite de 60 o 24
    if (minute >= 60) {
        hour += minute / 60;
        minute %= 60;
    } else if (minute < 0) {
        hour -= 1;
        minute += 60;
    }

    if (hour >= 24) {
        hour %= 24;
        day += 1;  // Incrementa el día si la hora supera las 24 horas
    } else if (hour < 0) {
        hour += 24;
        day -= 1;  // Ajuste si la hora es negativa
    }

    // Formatear la fecha y la hora en el estilo esperado
    String formattedDate = String(year) + (month < 10 ? "0" : "") + String(month) + (day < 10 ? "0" : "") + String(day);
    String formattedTime = (hour < 10 ? "0" : "") + String(hour) + ":" + (minute < 10 ? "0" : "") + String(minute) + ":" + (second < 10 ? "0" : "") + String(second);
    // Retorna la fecha y hora en formato UTC
    return formattedDate+DLM+formattedTime;
}
String Utils::getPositionData(String data,int position) {
  int startIndex = 0;
    int endIndex = 0;
    int currentPosition = 0;
    // Recorre la cadena buscando comas para separar en posiciones
    while (currentPosition <= position) {
        endIndex = data.indexOf(',', startIndex);  // Encuentra la siguiente coma

        // Si no encuentra más comas y estamos en la última posición, ajusta endIndex
        if (endIndex == -1) endIndex = data.length();

        // Si la posición actual coincide con la buscada, extrae el valor
        if (currentPosition == position) {
            return data.substring(startIndex, endIndex);  // Devuelve el valor en la posición deseada
        }

        // Actualiza el índice de inicio para la siguiente iteración
        startIndex = endIndex + 1;
        currentPosition++;
    }

    return "";
}

String Utils::cleanDelimiter(String str, const String &substring) {
    int index = str.indexOf(substring);  // Busca la subcadena
    if (index != -1) {  // Si se encuentra
        // Corta la cadena hasta la posición encontrada
        str = str.substring(0, index);  
    }
    return str;
}
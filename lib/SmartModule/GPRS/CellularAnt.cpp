#include "CellularAnt.h"

CellularAnt::CellularAnt(SIM7600& sim7600) : simModule(sim7600) {}

String CellularAnt::cellInformation(int timeout) {
    String cpsi_cmd = "AT+CPSI?";
    String cpsi = simModule.sendCommandWithResponse(cpsi_cmd.c_str(), timeout);
return cpsi;
}
std::vector<String> CellularAnt::splitString(const char *data, char delimiter) {
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

// Método público para parsear datos celulares
CellularAnt::CellularData CellularAnt::parse(const char *data) {
    CellularData cellData;

    // Dividir la cadena en tokens
    std::vector<String> tokens = splitString(data, ',');

    // Validar el número esperado de tokens (14 + 1 "OK" al final)
    if (tokens.size() >= 14) {
        cellData.systemMode = tokens[0];
        cellData.operationMode = tokens[1];

        // Dividir MCC y MNC
        int dashIndex = tokens[2].indexOf('-');
        cellData.mcc = tokens[2].substring(0, dashIndex);
        cellData.mnc = tokens[2].substring(dashIndex + 1);

        // Eliminar el prefijo "0x" de LAC si existe
        String lac = tokens[3];
        if (lac.startsWith("0x")) {
            cellData.lac = lac.substring(2);
        } else {
            cellData.lac = lac;
        }

        cellData.cellId = tokens[4];
        cellData.frequencyBand = tokens[5];
        cellData.psc = tokens[6].toInt();
        cellData.freq = tokens[7].toInt();
        cellData.ssc = tokens[8].toInt();
        cellData.ecIO = tokens[9].toFloat();
        cellData.rscp = tokens[10].toInt();
        cellData.qual = tokens[11].toInt();
        cellData.rxLev = tokens[12].toInt();
        cellData.txPwr = tokens[13].toInt();
    } else {
        Serial.println("Error: La cadena no contiene suficientes datos.");
    }

    return cellData;
}
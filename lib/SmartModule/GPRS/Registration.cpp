#include "Registration.h"

Registration::Registration(SIM7600& sim7600) : simModule(sim7600) {}

bool Registration::netRegistrationState() {
    String creg_cmd = "AT+CREG?";
    String creg = simModule.sendCommandWithResponse(creg_cmd.c_str(), 4000);
    Serial.println("AT+CREG => "+creg);
    if(creg == "0,1") {
        Serial.println("Modulo conectado a la red local");
        return true;
    }
    return false;
}

bool Registration::networkRegistration() {
    String cgact_cmd = "AT+CGACT=1,1";
    String cgact = simModule.sendCommandWithResponse(cgact_cmd.c_str(), 4000);
    Serial.println("AT+CGACT => "+cgact);
    if(cgact == "OK"){
        Serial.println("Comando "+cgact_cmd+" enviado exitosamente");
        return true;
    }else if(cgact == "+CME ERROR: unknown") {
        Serial.println("Error:  "+ cgact);
        return false;
    }
    return false;
}

void Registration::softReset() {
  String cfunr_cmd = "AT+CFUN=1,1";
  String cfunr = simModule.sendCommandWithResponse(cfunr_cmd.c_str(), 4000);
  Serial.println("CMD"+cfunr_cmd+"Rsp CMD => "+cfunr);

}

void Registration::apnRegistration(char *apn) {
    String apn_cmd = "AT+CGDCONT=1,\"IP\",\"" + String(apn) + "\"";
    String rsp_apn = simModule.sendCommandWithResponse(apn_cmd.c_str(), 4000);
    Serial.println("CMD"+apn_cmd+"Rsp CMD => "+rsp_apn);
}




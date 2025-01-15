#include "Registration.h"

Registration::Registration(SIM7600& sim7600) : simModule(sim7600) {}

bool Registration::netRegistrationState() {
    String creg_cmd = "AT+CREG?";
    String creg = simModule.sendCommandWithResponse(creg_cmd.c_str(), 4000);
    Serial.println("AT+CREG => "+creg);
    if(creg == "0,1") {
        Serial.println("Modulo registrado a la red!");
        return true;
    }
    return false;
}

void Registration::networkRegistration() {
    String cgact_cmd = "AT+CGACT=1,1";
    String cgact = simModule.sendCommandWithResponse(cgact_cmd.c_str(), 4000);
    Serial.println("AT+CGACT => "+cgact);
    if(cgact == "OK"){
        Serial.println("Comando "+cgact_cmd+" enviado exitosamente");
    }else if(cgact == "+CME ERROR: unknown") {
        Serial.println("Reiniciando Modulo... ");
    }
}
void Registration::softReset(){
  String cfunr_cmd = "AT+CFUN=1,1";
  String cfunr = simModule.sendCommandWithResponse(cfunr_cmd.c_str(), 4000);
  Serial.println("Rsp CMD => "+cfunr);

}
#ifndef CONFIG_H
#define CONFIG_H

#define SIM7600_RX_PIN 5     
#define SIM7600_TX_PIN 4     

#define AT_COMMAND_TIMEOUT 3000
#define SIM7600_BAUD_RATE 115200
#define SERIAL_BAUD_RATE 115200

#define POWER_KEY_PIN 41 
#define POWER_SIM_PIN 38
#define GNSS_LED_PIN  19
#define POWER_LED_PIN 20
#define SIM_DTR_PIN   42
#define INPUT_IGN   10   


#define DEFAULT_SVR "34.196.135.179"
#define DEFAULT_PORT 5200

#define SEND_DATA_TIMEOUT 1000
#define SEND_DATA_ING_OFF 600000 //5 minutos = 300000 ms //20 minutos = 1200000 ms
#define SEND_DATA_HEART_BEAT 588000 // 19.8  Minutos = 1188000 ms
//Static values
struct Headers {
    static constexpr const char *STT = "STT";
    static constexpr const char *ALT = "ALT";
    static constexpr const char *ALV = "ALV";
    static constexpr const char *RES = "RES";
    static constexpr const char *CMD = "CMD";
};
#define DLM ";" //delimiter
#define CHANGE_COURSE 20
#endif

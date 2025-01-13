#ifndef LITTLES_ENUMS_H
#define LITTLES_ENUMS_H

enum dateTimeType {
    DATE,
    TIME 
};

enum cmdType {
  TEST = 0,
  READ = 1,
  WRITE = 2,
  EXECUTE = 3,
  SEND = 4,
  UNKNOWN = -1
};
enum AlertId {
  IGNITON_ON        = 33, 
  IGNITON_OFF       = 34,   
  POWER_CONNECTED   = 40,
  POWER_DISCONECTED = 41,
  BATTERY_CONNECTED = 44,
  BATT_DISCONECTED  = 45,
  INPUT_HIGH        = 11,
  INPUT_LOW         = 12,
  SHOCKED           = 15,
  CRASH_DETECTED    = 16,
  PANIC_BUTTON      = 42,
};
#endif
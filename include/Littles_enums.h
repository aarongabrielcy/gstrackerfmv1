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
  IGN_ON    = 33, 
  IGN_OFF   = 34,   
  POWER_ON  = 40,
  POWER_OFF = 41,
  BATT_ON   = 44,
  BATT_OFF  = 45,
  IN_1_ON   = 11,
  IN_1_OFF  = 12,
  PANIC_BTN = 42,
};
#endif
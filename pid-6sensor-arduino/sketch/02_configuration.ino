#include <EEPROM.h>
#include <avr/pgmspace.h>

typedef struct {
  float p, i, d;
} t_pid;

typedef struct {
  // total configuration structure lenght including this byte
  short length;
  byte version;
  // ------- actual configuration --------
  // line following parameters
  t_pid lineFollowPID;
  float minSpeedAngle;
  float minSpeed;
  float maxSpeed;
  uint16_t minimumContrast;
  int equalizationBackground[nSensors];
  int equalizationForeground[nSensors];
  // -------------------------------------
  int8_t checksum;
} t_config;

t_config config = {sizeof(t_config), 1, {0.02, 0.0, 0.0}, 0.3, 0.4, 1.0, 200, 
  {1023, 1023, 1023, 1023, 1023, 1023},
  {0, 0, 0, 0, 0, 0},
  0};

bool readConfigFromFlash() {
  int8_t checksum = 0;
  t_config c;
  byte *pc = (byte *)&c;
  for(short i = 0; i < sizeof(t_config); i++) {
    *pc = EEPROM.read(i);
    checksum += *pc;
    pc++;
  }
  // validate config
  if(c.length != sizeof(t_config) || checksum != 0) {
    return false;
  }
  // copy config to public field
  memcpy(&config, &c, sizeof(t_config));
  return true;
}

void writeConfigToFlash() {
  int8_t checksum = 0;
  t_config c;
  byte *pc = (byte *)&config;
  for(short i = 0; i < sizeof(t_config) - 1; i++) {
    checksum += *pc;
    EEPROM.update(i, *pc);
    pc++;
  }
  checksum = -checksum;
  EEPROM.write(sizeof(t_config) - 1, checksum);
}

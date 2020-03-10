 #include <Fsm.h>

#include <avr/pgmspace.h>
#include <MenuSystem.h>

// **** global configuration constants *****
//#define DEBUG_SHOW_CYCLES_PER_SECOND

// sensor led enable pin - active low
#define SENSOR_LED_EN_PIN 7
// robot autonomous drive start/stop button - active low
#define BUTTON_PIN 2
// number of drive cycles when it's detemined that line is lost
static const uint16_t LOW_CONTRAST_STOP_CYCLES = 20;

static const int CALIBRATED_MAX_SENSOR_VALUE = 300;

static const bool ENABLE_CALIBRATED_RANGE_CLIPPING = true;

// analog input for sensor pins in the order left to right (looking from robot back)
// led weights:
// S1   S2   S3   S4   S5   S6   S7   S8   S9   S10  S11  S12  S13  S14
// -40  -35  -29  -22  -16  -10  -3   3    10   16   22   28   35   40
// S1   S2   S3   S4   S5   S6   S7   S8   S9   S10  S11  S12  S13  S14
// -40  -35  -29  -22  -16  -10  -3   3    10   16   22   28   35   40
static const byte sensorPins[] = {A5, A4, A3, A2, A0, A1};
static const int8_t sensorWeights[] = {-35, -22, -10, 10, 22, 35};
static const byte nSensors = sizeof(sensorPins);
int sensorRaw[nSensors];
int sensor[nSensors];
// *****************************************
#ifdef DEBUG_SHOW_CYCLES_PER_SECOND
uint16_t cyclesCounter = 0;
unsigned long cyclesCounterTimestamp = 0;
#endif

void setup() {
  // debug output
  Serial.begin(9600);
  if(readConfigFromFlash() == false) {
    Serial.println(F("Config invalid, recalibrate!!!"));
  }
  // sensor analog inputs
//  analogReference(INTERNAL);
  // sensor led control. LOW = LED on, HIGH = LED off
  pinMode(SENSOR_LED_EN_PIN, OUTPUT);
  digitalWrite(SENSOR_LED_EN_PIN, LOW);
  // modules setup
  setupMotors();
  setupMenu();
  // FSM module is responsible for initializing all FSMs
  setupFsm();
}

void readSensorInputs(int sensor[]) {
  for(byte i = 0; i < sizeof(sensorPins); i++) {
    sensor[i] = analogRead(sensorPins[i]);
  }
}

void loop() {
  readSensorInputs(sensorRaw);
  equalizeSensors(sensorRaw, sensor, nSensors);
  
  handleMenu();
  handleFsm();
  delay(10);
#ifdef DEBUG_SHOW_CYCLES_PER_SECOND
  cyclesCounter++;
  if(millis() - cyclesCounterTimestamp >= 1000) {
    Serial.print(F("cps="));
    Serial.println(cyclesCounter);
    cyclesCounter = 0;
    cyclesCounterTimestamp = millis();
  }
#endif
}

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
 
int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
    

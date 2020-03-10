enum e_calibrationEvents {CALIBRATION_DONE, CALIBRATION_CONTRAST, CALIBRATION_SENSOR_NEXT};

State calibrationIdle(onCalibIdleEnter, NULL, NULL);
State calibrationSensor(onCalibSensorEnter, stepCalibSensor, NULL);
State calibrationBackground(onCalibAverageEnter, stepCalibGetAverages, onCalibBgExit);
State calibrationForeground(onCalibAverageEnter, stepCalibGetAverages, onCalibFgExit);

Fsm calibrationFsm(&calibrationIdle);

unsigned long calStartTimestamp;

void onCalibIdleEnter() {
  driveFsm.trigger(STOP);
}

/** sensor contrast calibration routine **/
enum e_calSensorStage {CAL_SENSOR_STARTING, CAL_SENSOR_ROTATING_LEFT, CAL_SENSOR_STOPPING, CAL_SENSOR_ROTATING_RIGHT, CAL_SENSOR_DONE};

e_calSensorStage calSensorStage;
int minContrast, maxContrast;

void onCalibSensorEnter() {
  minContrast = 0xFFFF;
  maxContrast = 0;
  calSensorStage = CAL_SENSOR_STARTING;
}

void stepCalibSensor() {
  int min, max;
  int contrast;
  switch(calSensorStage) {
    case CAL_SENSOR_STARTING:
      driveFsm.trigger(DRIVE);
      // rotate left 
      setDirection(-1.0);
      setSpeed(0.3);
      calStartTimestamp = millis();
      calSensorStage = CAL_SENSOR_ROTATING_LEFT;
      break;
    case CAL_SENSOR_ROTATING_LEFT:
      contrast = calcContrast(sensor, nSensors, min, max);
      minContrast = min(contrast, minContrast);
      maxContrast = max(contrast, maxContrast);
      if(millis() - calStartTimestamp > 2000) {
        setSpeed(0.0);
        calStartTimestamp = millis();
        calSensorStage = CAL_SENSOR_STOPPING;
      }
      break;
    case CAL_SENSOR_STOPPING:
      if(millis() - calStartTimestamp > 200) {
        setDirection(1.0);
        setSpeed(0.3);
        calStartTimestamp = millis();
        calSensorStage = CAL_SENSOR_ROTATING_RIGHT;
      }
      break;
    case CAL_SENSOR_ROTATING_RIGHT:
      if(millis() - calStartTimestamp > 2000) {
        driveFsm.trigger(STOP);
        Serial.print("min = ");
        Serial.print(minContrast);
        Serial.print(", max = ");
        Serial.println(maxContrast);
        calibrationFsm.trigger(CALIBRATION_DONE);
      }
      break;
  }
}

/** sensor background and foreground calibration routine **/
int sensorRawCalibration[nSensors];
int sensorRawCalibrationStdDev[nSensors];
byte nMeasurements;
static const unsigned long N_AVERAGES = 32;

void onCalibAverageEnter() {
  calStartTimestamp = millis();
  calSensorStage = CAL_SENSOR_STARTING;
  nMeasurements = 1;
  memcpy(sensorRawCalibration, sensorRaw, sizeof(sensorRawCalibration));
  memset(sensorRawCalibrationStdDev, 0, sizeof(sensorRawCalibrationStdDev));
}

void stepCalibGetAverages() {
  switch(calSensorStage) {
    case CAL_SENSOR_STARTING:
      if(millis() - calStartTimestamp > 100) {
        calStartTimestamp = millis();
        for(byte i = 0; i < nSensors; i++) {
          sensorRawCalibration[i] += sensorRaw[i];
        }
        nMeasurements++;
        if(nMeasurements == N_AVERAGES) {
          calSensorStage = CAL_SENSOR_STOPPING;
        }
      }
      break;
    case CAL_SENSOR_STOPPING:
      for(byte i = 0; i < nSensors; i++) {
        sensorRawCalibration[i] /= nMeasurements;
      }
      printCalibrationAverage();
      calSensorStage = CAL_SENSOR_DONE;
      break;
    case CAL_SENSOR_DONE:
      break;
  }
}

void onCalibBgExit() {
  memcpy(config.equalizationBackground, sensorRawCalibration, sizeof(config.equalizationBackground));
}

void onCalibFgExit() {
  memcpy(config.equalizationForeground, sensorRawCalibration, sizeof(config.equalizationForeground));
  printDynamicRange();
}

void printCalibrationAverage() {
      // calculate avaerages
      Serial.print(F("Average after "));
      Serial.print(nMeasurements);
      Serial.print(F(" measurements=["));
      for(byte i = 0; i < nSensors; i++) {
        Serial.print(sensorRawCalibration[i]);
        Serial.print(F(" "));
      }
      Serial.println(F("]"));
}

void printDynamicRange() {
      Serial.print(F("Dynamic range=["));
      for(byte i = 0; i < nSensors; i++) {
        Serial.print(config.equalizationForeground[i] - config.equalizationBackground[i]);
        Serial.print(F(" "));
      }
      Serial.println(F("]"));
}

void setupCalibrationFsm() {
  calibrationFsm.add_transition(&calibrationIdle, &calibrationSensor, CALIBRATION_CONTRAST, NULL);
  calibrationFsm.add_transition(&calibrationIdle, &calibrationBackground, CALIBRATION_SENSOR_NEXT, NULL);
  calibrationFsm.add_transition(&calibrationBackground, &calibrationForeground, CALIBRATION_SENSOR_NEXT, NULL);
  calibrationFsm.add_transition(&calibrationForeground, &calibrationIdle, CALIBRATION_SENSOR_NEXT, NULL);
  calibrationFsm.add_transition(&calibrationSensor, &calibrationIdle, CALIBRATION_DONE, NULL);
}

enum e_driveEvents {DRIVE, STOP, FOLLOW_LINE};

State driveStopped(onDriveStoppedEnter, NULL, NULL);
State driveEnabled(onDriveEnter, stepDrive, NULL);
State driveEnabledFollow(onFollowLineEnter, stepFollowLine, NULL);

Fsm driveFsm(&driveStopped);
float targetSpeed, targetAngle;
float angleErrorIntegral = 0.0;
uint8_t lowContrastCounter;

// DEBUG
unsigned long motorStreamingTimestamp = 0;
void streamMotorDriveData(float l, float r) {
  if(millis() - motorStreamingTimestamp > STREAMING_INTERVAL_MS) {
    motorStreamingTimestamp = millis();
    Serial.print(l);
    Serial.print(F(" "));
    Serial.print(r);
    Serial.print(F(" "));
    Serial.print(targetSpeed);
    Serial.print(F(" "));
    Serial.print(targetAngle);
    Serial.print(F(" "));
    Serial.println(angleErrorIntegral);
  }
}

float mapf(float val, float in_min, float in_max, float out_min, float out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void onDriveStoppedEnter() {
  stopMotors();
}

void onDriveEnter() {
  targetSpeed =
    targetAngle = 0;
}

void stepDrive() {
  float l = 1.0, r = 1.0;
  // turning right - left motor full speed, right motor regulated
  // turning left - right motor full speed, left motor regulated
  if(targetAngle >= 0) {
    r = 2 * (0.5 - targetAngle);
  } else {
    l = 2 * (0.5 + targetAngle);
  }
  // scale by speed factor
  r *= targetSpeed;
  l *= targetSpeed;
  // DEBUG
  streamMotorDriveData(l, r);
  setLeftSpeed(l);
  setRightSpeed(r);
}

void onFollowLineEnter() {
  onDriveEnter();
  angleErrorIntegral =
    lowContrastCounter = 0;
}

void stepFollowLine() {
  float cow = calcCenterOfWeight(sensor, nSensors);
  float angleError = cow;
  angleErrorIntegral += angleError * config.lineFollowPID.i;
  setDirection(angleError * config.lineFollowPID.p + angleErrorIntegral);
  float speed = max(mapf(abs(targetAngle), 0, config.minSpeedAngle, config.maxSpeed, config.minSpeed), config.minSpeed);
  setSpeed(speed);
  stepDrive();
  checkContrast();
}

void checkContrast() {
  int min, max;
  if(calcContrast(sensor, nSensors, min, max) < config.minimumContrast) {
    lowContrastCounter++;
    if(lowContrastCounter > LOW_CONTRAST_STOP_CYCLES) {
      driveFsm.trigger(STOP);
    }
  } else {
    lowContrastCounter = 0;
  }
}

void setupDriveFsm() {
  driveFsm.add_transition(&driveStopped, &driveEnabled, DRIVE, NULL);
  driveFsm.add_transition(NULL, &driveStopped, STOP, NULL);
  driveFsm.add_transition(&driveStopped, &driveEnabledFollow, FOLLOW_LINE, NULL);
  driveFsm.add_transition(&driveEnabledFollow, &driveStopped, FOLLOW_LINE, NULL);
}

void setSpeed(float speed) {
  speed = constrain(speed, 0.0f, 1.0f);
  targetSpeed = speed;
}

/**
 * @param angle -1 - 1
 */
void setDirection(float angle) {
  angle = constrain(angle, -1.0f, 1.0f);
  targetAngle = angle;
}

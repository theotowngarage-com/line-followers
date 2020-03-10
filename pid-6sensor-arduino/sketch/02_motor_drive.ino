#define MOTOR_R_DIR_PIN 9
#define MOTOR_R_SPEED_PIN 6

#define MOTOR_L_DIR_PIN 4
#define MOTOR_L_SPEED_PIN 5

void setupMotors() {
  // configure motors
  pinMode(MOTOR_L_DIR_PIN, OUTPUT);
  pinMode(MOTOR_L_SPEED_PIN, OUTPUT);
  pinMode(MOTOR_R_DIR_PIN, OUTPUT);
  pinMode(MOTOR_R_SPEED_PIN, OUTPUT);
  stopMotors();
}

void stopMotors() {
  analogWrite(MOTOR_L_SPEED_PIN, 0);
  analogWrite(MOTOR_R_SPEED_PIN, 0);
}

/**
 * Set left motor speed and direction
 * Set negative speed to drive backwards, and positive speed to drive forward.
 * /param speed target motor speed -1.0 to 1.0. 
 */
void setLeftSpeed(float speed) {
  speed = constrain(speed, -1.0, 1.0);
  if(speed >= 0) {
    digitalWrite(MOTOR_L_DIR_PIN, HIGH);
    analogWrite(MOTOR_L_SPEED_PIN, (int)(speed * 255));
  } else {
    digitalWrite(MOTOR_L_DIR_PIN, LOW);
    analogWrite(MOTOR_L_SPEED_PIN, (int)(-speed * 255));
  }
}

/**
 * Set left motor speed and direction
 * Set negative speed to drive backwards, and positive speed to drive forward.
 * /param speed target motor speed -1.0 to 1.0. 
 */
void setRightSpeed(float speed) {
  speed = constrain(speed, -1.0, 1.0);
  if(speed >= 0) {
    digitalWrite(MOTOR_R_DIR_PIN, LOW);
    analogWrite(MOTOR_R_SPEED_PIN, (int)(speed * 255));
  } else {
    digitalWrite(MOTOR_R_DIR_PIN, HIGH);
    analogWrite(MOTOR_R_SPEED_PIN, (int)(-speed * 255));
  }
}

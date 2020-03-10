void setupFsm() {
  setupStreamingFsm(); 
  setupDriveFsm();
  setupCalibrationFsm();
}

// runs idle tasks for all fsm machines
void handleFsm() {
  driveFsm.run_machine();  
  streamingFsm.run_machine();
  calibrationFsm.run_machine();
}

void delayWithFsm(unsigned long delayMillis) {
  unsigned long startTime = millis();
  while(millis() - startTime < delayMillis) {
    handleFsm();
  }
}

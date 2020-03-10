#define STREAMING_INTERVAL_MS 100
enum e_dataStream {NONE, RAW_SENSOR, EQ_SENSOR, SENSOR_STATISTICS, SENSOR_HEADING };

State streamNone(NULL, NULL, NULL);
State streamRawSensorData(NULL, stepStreamRawSensorData, NULL);
State streamEqualizedSensorData(NULL, stepStreamEqualizedSensorData, NULL);
State streamHeading(NULL, stepStreamHeading, NULL);
State streamStatistics(NULL, stepStreamStatistics, NULL);

Fsm streamingFsm(&streamNone);
unsigned long streamingTimestamp = 0;

void stepStreamRawSensorData() {
  if(millis() - streamingTimestamp > STREAMING_INTERVAL_MS) {
    streamingTimestamp = millis();
    for(byte i = 0; i < nSensors; i++) {
      Serial.print(sensorRaw[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void stepStreamEqualizedSensorData() {
  if(millis() - streamingTimestamp > STREAMING_INTERVAL_MS) {
    streamingTimestamp = millis();
    for(byte i = 0; i < nSensors; i++) {
      Serial.print(sensor[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void stepStreamHeading() {
  if(millis() - streamingTimestamp > STREAMING_INTERVAL_MS) {
    streamingTimestamp = millis();
    float cow = calcCenterOfWeight(sensor, nSensors);
    Serial.println(cow);
  }
}

void stepStreamStatistics() {
  if(millis() - streamingTimestamp > STREAMING_INTERVAL_MS) {
    streamingTimestamp = millis();
    int min, max;
    int contrast = calcContrast(sensor, nSensors, min, max);
    Serial.print(min);
    Serial.print(F(" "));
    Serial.print(max);
    Serial.print(F(" "));
    Serial.print(contrast);
    Serial.print(F(" "));
    Serial.println(totalWeight(sensor, nSensors));
    
  }
}

void setupStreamingFsm() {
  streamingFsm.add_transition(NULL, &streamNone, NONE, NULL);
  streamingFsm.add_transition(NULL, &streamRawSensorData, RAW_SENSOR, NULL);
  streamingFsm.add_transition(NULL, &streamEqualizedSensorData, EQ_SENSOR, NULL);
  streamingFsm.add_transition(NULL, &streamHeading, SENSOR_HEADING, NULL);
  streamingFsm.add_transition(NULL, &streamStatistics, SENSOR_STATISTICS, NULL);
}

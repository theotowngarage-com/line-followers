int totalWeight(int sensor[], const byte n) {
  int sum = 0;
  for(byte i = 0; i < n; i++) {
    sum += sensor[i];
  }
  return sum;
}

float calcCenterOfWeight(int sensor[], const byte n) {
  int32_t nominator = 0;
  int32_t denominator = 0;
  for(byte i = 0; i < n; i++) {
    denominator += sensor[i];
    nominator += sensorWeights[i] * sensor[i];
  }
  if(denominator == 0)
    return 0;
  return (float)nominator / denominator;
}

int calcContrast(int sensor[], const byte n, int &min, int &max) {
  min = sensor[0];
  max = sensor[0];
  for(byte i = 1; i < n; i++) {
    if(sensor[i] > max) {
      max = sensor[i];
    } else if(sensor[i] < min) {
      min = sensor[i];
    }
  }
  return max - min;
}

void equalizeSensors(int sensorIn[], int sensorOut[], byte n)
{
  int *eqBg = config.equalizationBackground;
  int *eqFg = config.equalizationForeground;
  for(byte i = 0; i < n; i++) {
    sensorOut[i] = map(sensorIn[i], eqFg[i], eqBg[i], CALIBRATED_MAX_SENSOR_VALUE, 0);
    if(ENABLE_CALIBRATED_RANGE_CLIPPING) {
      sensorOut[i] = constrain(sensorOut[i], 0, CALIBRATED_MAX_SENSOR_VALUE);
    }
  }
}

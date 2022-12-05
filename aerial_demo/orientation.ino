#define alpha_pitch 0.98
#define alpha_roll 0.98

#define rad2deg 180/PI

float accel_calib[] = {0.0, 0.0, 0.0};
float gyro_calib[] = {0.0, 0.0, 0.0};

float pitch;
float roll;

void calibrateIMU(int samples) {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  for (int i = 0; i < samples; i++) {
    lsm6ds3trc.getEvent(&accel, &gyro, &temp);
    accel_calib[0] += accel.acceleration.x;
    accel_calib[1] += accel.acceleration.y;
    accel_calib[2] += accel.acceleration.z;
    gyro_calib[0] += gyro.gyro.x;
    gyro_calib[1] += gyro.gyro.y;
    gyro_calib[2] += gyro.gyro.z;
  }

  accel_calib[0] /= samples;
  accel_calib[1] /= samples;
  accel_calib[2] /= samples;
  gyro_calib[0] /= samples;
  gyro_calib[1] /= samples;
  gyro_calib[2] /= samples;
}

void initFilters() {
  pitch = 0;
  roll = 0;
}

float getPitch() {
  return pitch;
}

float getRoll() {
  return roll;
}

void updateFilters(sensors_event_t gyro, sensors_event_t accel, float dt) {
  updatePitch(gyro, accel, dt);
  updateRoll(gyro, accel, dt);
}

void updatePitch(sensors_event_t gyro, sensors_event_t accel, float dt) {
  pitch = alpha_pitch * (getPitch() + gyro.gyro.y * dt) + (1 - alpha_pitch)*rad2deg*atan(accel.acceleration.x/accel.acceleration.z);
}

void updateRoll(sensors_event_t gyro, sensors_event_t accel, float dt) {
  roll = alpha_roll * (getRoll() + gyro.gyro.x * dt)  + (1 - alpha_roll)*rad2deg*atan(accel.acceleration.y/accel.acceleration.z);
}
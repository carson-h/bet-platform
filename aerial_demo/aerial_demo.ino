#include <SPI.h>
#include "printf.h"
#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include <Adafruit_LSM6DS3TRC.h>
#include <Servo.h>

#define DEBUG 1
#define RETRANS_LIMIT 50

bool status;

// IR Camera
Adafruit_AMG88xx amg;

float pixels[AMG88xx_PIXEL_ARRAY_SIZE];

// IMU
Adafruit_LSM6DS3TRC lsm6ds3trc;

float ori[] = {0.0, 0.0}; // Orientation set point
unsigned long lastIMU;

char command = 0x00; // Command from ground station

// Gimbal Control
Servo pitchServo; //0-90deg
Servo rollServo; // 0-115deg

#if DEBUG
#define PITCH_GIMBAL_GAIN 0.1
#define ROLL_GIMBAL_GAIN 0.1
#else
#define PITCH_GIMBAL_GAIN 0.08
#define ROLL_GIMBAL_GAIN 0.05
#endif

#define PITCH_SERVO_LOW 0
#define PITCH_SERVO_HIGH 90

#define ROLL_SERVO_LOW 0
#define ROLL_SERVO_HIGH 115

float limits(float value, float low, float high) {
  if (value < low)
    return low;
  else if (value > high)
    return high;
  else
    return value;
}

int getResponseSize(char command) {
  // status byte + amg8833 + hc sr-04 + lsm6ds3tr-c
  int length = 1; // status byte

  if (0b10000000 & command) {
    length += AMG88xx_PIXEL_ARRAY_SIZE*sizeof(float);
  }

  if (0b01000000 & command) {
    length += sizeof(float);
  }

  if (0b00100000 & command) {
    length += 2*sizeof(float);
  }
  
  return length;
}

char measureBattery() {
  return 0;
}

void setup() {
  Serial.begin(115200, SERIAL_8O1);
  while (!Serial); // some boards need to wait to ensure access to serial over USB
  
  status = amg.begin();
  if (!status)
    while (1);

  status = lsm6ds3trc.begin_I2C();
  if (!status)
    while (1);

  // Configure accelerometer and gyroscope
  lsm6ds3trc.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  lsm6ds3trc.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
  lsm6ds3trc.setAccelDataRate(LSM6DS_RATE_1_66K_HZ);
  lsm6ds3trc.setGyroDataRate(LSM6DS_RATE_1_66K_HZ);

  lsm6ds3trc.configInt1(false, false, true); // accelerometer DRDY on INT1
  lsm6ds3trc.configInt2(false, true, false); // gyro DRDY on INT2

  // Initialize complementary filters for determining orientation from gyro and accel data
  calibrateIMU(50);
  initFilters();
  lastIMU = micros();

  pitchServo.attach(5);
  rollServo.attach(6);
}

void loop() {
  // Get command message
  if (Serial.available()) {
    command = Serial.read();

    // SORI bit set
    // Recieve the remaining orientation information
    if (0b00010000 & command) {
      while (Serial.available() < 2)
      ori[0] = Serial.read();
      ori[1] = Serial.read();
    }

    int length = getResponseSize(command);
    char r_buf[length];
    handleCommand(command, r_buf);

    Serial.write(r_buf, length);
  }

  // Gimbaling

  // Get orientation
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  lsm6ds3trc.getEvent(&accel, &gyro, &temp);
  unsigned long newIMU = micros();
  //Serial.println(newIMU-lastIMU);
  updateFilters(gyro, accel, (newIMU-lastIMU)*0.000001);
  lastIMU = newIMU;

  // Adjust servo position
  #if !DEBUG
  pitchServo.write(limits(PITCH_GIMBAL_GAIN*(ori[0]-getPitch())+pitchServo.read(), PITCH_SERVO_LOW, PITCH_SERVO_HIGH));
  rollServo.write(limits(ROLL_GIMBAL_GAIN*(ori[1]-getRoll())+rollServo.read(), ROLL_SERVO_LOW, ROLL_SERVO_HIGH));
  #endif
}

// Responds to the command provided by the wireless module.
void handleCommand (char command, char* buffer) {
  int index = 0;
  
  // AMG8833
  if (0b10000000 & command) {
    // Read all the pixels
    amg.readPixels(pixels);

    memcpy(buffer+index, pixels, AMG88xx_PIXEL_ARRAY_SIZE*sizeof(float));

    index += AMG88xx_PIXEL_ARRAY_SIZE*sizeof(float);
  }

  // HC-SR04
  if (0b01000000 & command) {
    // Get distance

    // Not implemented due to issues with interface
    float distance = 0.0;
    memcpy(buffer+index, &distance, sizeof(float));

    index += sizeof(float);
  }

  // LSM6DS3TR-C
  if (0b00100000 & command) {
    // Get a new sensor event
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    lsm6ds3trc.getEvent(&accel, &gyro, &temp);
    unsigned long newIMU = micros();
    updateFilters(gyro, accel, (newIMU-lastIMU)*0.000001);
    lastIMU = newIMU;

    // Copy to buffer
    float val = getPitch();
    memcpy(buffer+index, &val, sizeof(float));
    index += sizeof(float);
    val = getRoll();
    memcpy(buffer+index, &val, sizeof(float));
    index += sizeof(float);
  }
  
  // Create status byte (status and battery measurement not implement yet)

  // TODO Implement stability assessment
  buffer[index] = measureBattery() << 4;
}

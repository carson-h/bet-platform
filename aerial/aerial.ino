#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include <Adafruit_LSM6DS3TRC.h>
#include <Servo.h>

#define DEBUG 1

bool status;

// IR Camera
Adafruit_AMG88xx amg;

float pixels[AMG88xx_PIXEL_ARRAY_SIZE];

// IMU
Adafruit_LSM6DS3TRC lsm6ds3trc;

float ori[] = {0.0, 0.0}; // Orientation set point
unsigned long lastIMU;

// Radio
RF24 radio(7, 8);  // using pin 7 for the CE pin, and pin 8 for the CSN pin

uint8_t address[][6] = { "GCONT", "ADATA" }; // ? DOES AUTOACK NEED DISABLED FOR THE GCONT ADDRESS TO BE USEFUL. WHAT NEEDS TO BE DONE TO KEEP THIS WORKING
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
  #if DEBUG
  Serial.begin(115200);
  while (!Serial); // some boards need to wait to ensure access to serial over USB
  #endif

  status = amg.begin();
  if (!status) {
    #if DEBUG
    Serial.println("Failed to find a valid AMG8833.");
    #endif
    while (1);
  }

  status = lsm6ds3trc.begin_I2C();
  if (!status) {
    #if DEBUG
    Serial.println("Failed to find a valid LSM6DS3TR-C.");
    #endif
    while (1);
  }

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

  // Initialize the transceiver on the SPI bus
  status = radio.begin();
  if (!status) {
    #if DEBUG
    Serial.println(F("Failed to find a valid nRF24L01+."));
    #endif
    while (1);
  }

  // TODO Determine if the radio power parameter needs changed on these devices. (Configurable for debug mode?)
  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

  // TODO Fixed payload size? Is variable width possible? What is the most effective width?
  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, address[0]); // set the RX address of the TX node into a RX pipe
  radio.startListening();

  pitchServo.attach(5);
  rollServo.attach(6);
}

void loop() {
  // Get command message
  if (radio.available()) {
    radio.read(&command, 1); // read command byte

    #if DEBUG
    Serial.print(F("Received: "));
    Serial.println((byte)command);  // print the payload's value
    #endif

    // SORI bit set
    // Recieve the remaining orientation information
    if (0b00010000 & command) {
      while (!radio.available() && radio.getPayloadSize()); // Wait until information is available
      radio.read(ori, 2*sizeof(float)); // Read orientation
    }

    radio.stopListening();

    int length = getResponseSize(command);
    char r_buf[length];
    handleCommand(command, r_buf);

    radio.openWritingPipe(address[1]);
    radio.writeBlocking(r_buf, length, 500); // Load buffers for up to 0.5s
    radio.txStandBy(500); // Retry transmission for up to 0.5s or ACK received
    #if DEBUG
    if (status) {
      Serial.print("Handled command with output length: ");
      Serial.println(length);  // print payload sent
      Serial.println(r_buf);
    }
    else
      Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
    #endif

    radio.startListening();
  }

  #if DEBUG
  //if (!radio.available()) {
    //int length = getResponseSize(0b11100000);
    //char r_buf[length];
    //handleCommand(0b11100000, r_buf);
    //Serial.print("Handled command with output length: ");
    //Serial.println(length);
  //}
  #endif

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

  #if DEBUG
  //Serial.print("PITCH: ");
  //Serial.println(getPitch());
  //Serial.print("ROLL:");
  //Serial.println(getRoll());
  #endif

  // Adjust servo position
  pitchServo.write(limits(PITCH_GIMBAL_GAIN*(ori[0]-getPitch())+pitchServo.read(), PITCH_SERVO_LOW, PITCH_SERVO_HIGH));
  rollServo.write(limits(ROLL_GIMBAL_GAIN*(ori[1]-getRoll())+rollServo.read(), ROLL_SERVO_LOW, ROLL_SERVO_HIGH));
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

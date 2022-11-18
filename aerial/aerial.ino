/* 
Aerial Platform Code

TODO Kalman filtering 

*/

#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include <Adafruit_LSM6DS3TRC.h>

#define DEBUG 1

Adafruit_AMG88xx amg;

float pixels[AMG88xx_PIXEL_ARRAY_SIZE];

Adafruit_LSM6DS3TRC lsm6ds3trc;

// Instantiate an object for the nRF24L01 transceiver
RF24 radio(7, 8);  // using pin 7 for the CE pin, and pin 8 for the CSN pin

// nRF24 tranciever addresses
uint8_t address[][6] = { "GCONT", "ADATA" }; // ? DOES AUTOACK NEED DISABLED FOR THE GCONT ADDRESS TO BE USEFUL. WHAT NEEDS TO BE DONE TO KEEP THIS WORKING

char command = 0x00; // Command from ground station
float ori[] = {0.0, 0.0}; // Orientation set point

bool status;

int getResponseSize(char command) {
  // status byte + amg8833 + hc sr-04 + lsm6ds3tr-c
  return 1 + (0b10000000 & 1 != 0)*AMG88xx_PIXEL_ARRAY_SIZE*sizeof(float) + (0b01000000 & 1 != 0)*sizeof(float) + (0b00100000 & 1 != 0)*2*sizeof(float);
}

char measureBattery() {
  return 0;
}

void setup() {
  if (DEBUG) {
    Serial.begin(115200);
    while (!Serial); // some boards need to wait to ensure access to serial over USB
  }

  status = amg.begin();
  if (!status) {
    if (DEBUG) {
      Serial.println("Failed to find a valid AMG8833.");
    }
    while (1);
  }

  status = lsm6ds3trc.begin_I2C();
  if (!status) {
    if (DEBUG) {
      Serial.println("Failed to find a valid LSM6DS3TR-C.");
    }
    while (1);
  }

  // Configure accelerometer and gyroscope
  lsm6ds3trc.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  lsm6ds3trc.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
  lsm6ds3trc.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
  lsm6ds3trc.setGyroDataRate(LSM6DS_RATE_12_5_HZ);

  lsm6ds3trc.configInt1(false, false, true); // accelerometer DRDY on INT1
  lsm6ds3trc.configInt2(false, true, false); // gyro DRDY on INT2

  // Initialize the transceiver on the SPI bus
  status = !radio.begin();
  if (!status) {
    if (DEBUG) {
      Serial.println(F("Failed to find a valid nRF24L01+."));
    }
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
  //radio.setPayloadSize(sizeof(payload));  // float datatype occupies 4 bytes
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, address[0]); // set the RX address of the TX node into a RX pipe
  radio.startListening();
}

void loop() {
  // Get command message
  if (radio.available()) {
    radio.read(&command, 1); // read command byte

    if (DEBUG) {
      Serial.print(F("Received: "));
      Serial.println(command);  // print the payload's value
    }

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
    if (DEBUG) {
      if (status)
        Serial.println(r_buf);  // print payload sent
      else
        Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
    }

    radio.startListening();
  }

  // Gimbaling

}

void handleCommand (char command, char* buffer) {
  int index = 0;
  
  // AMG8833
  if (0b10000000 & command) {
    // Read all the pixels
    amg.readPixels(pixels);

    memcpy(buffer, pixels, AMG88xx_PIXEL_ARRAY_SIZE*sizeof(float));

    index += AMG88xx_PIXEL_ARRAY_SIZE*sizeof(float);
  }

  // HC-SR04
  if (0b01000000 & command) {
    // Get distance
    // TODO get this to work

    index += sizeof(float);
  }

  // LSM6DS3TR-C
  if (0b00100000 & command) {
    // Get a new normalized sensor event
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    lsm6ds3trc.getEvent(&accel, &gyro, &temp);

    // TODO Get information from gyro memcpy(buffer, , 2*sizeof(float));

    index += 2*sizeof(float);
  }
  
  // Create status byte (status and battery measurement not implement yet)

  // TODO Implement stability assessment
  // TODO implement
  buffer[index] = 0;
  
}

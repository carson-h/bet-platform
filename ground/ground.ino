#include <SPI.h>
#include "RF24.h"
#include <Adafruit_AMG88xx.h> // For array size constant


RF24 radio(4, 5);  // using GPIO4 for the CE pin, and GPIO5 for the CSN pin. Note these are different than the pins labelled on the NodeMCU module

uint8_t address[][6] = { "GCONT", "ADATA" };

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

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize nRF24
  if (!radio.begin()) {
    Serial.println(F("Failed to find a valid nRF24L01+"));
    while (1); // hold
  }

  
  // role variable is hardcoded to RX behavior, inform the user of this
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.enableDynamicPayloads();

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[0]);  // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  // additional setup specific to the node's role
  radio.stopListening();
}

void loop() {
  if (Serial.available()) {
    char command;
    Serial.readBytes(&command, 1);
    
    radio.writeBlocking(&command, 1, 500);
    radio.txStandBy(500);
    
    char ori[2*sizeof(float)];
    if (0b00010000 & command) {
      Serial.readBytes(ori, 2*sizeof(float));
    }

    radio.writeBlocking(ori, 2*sizeof(float), 500);
    radio.txStandBy(500);

    radio.startListening();

    int length = getResponseSize(command);
    char buffer[length];

    radio.read(buffer, length);

    Serial.write(buffer, length);
  }
  delay(15);
}

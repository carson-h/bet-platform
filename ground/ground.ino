#include <SPI.h>
#include "RF24.h"
#include <Adafruit_AMG88xx.h> // For array size constant

#define DEBUG 1
#define RETRANS_LIMIT 50

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
  Serial.begin(115200, SERIAL_8O1);
  while (!Serial);

  // Initialize nRF24
  if (!radio.begin()) {
    Serial.println(F("Failed to find a valid nRF24L01+"));
    while (1); // hold
  }

  radio.enableDynamicPayloads();

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[0]);

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[1]);

  radio.stopListening();
}

char command;
char ori[2*sizeof(float)];
int length;

void loop() {
  if (Serial.available()) {
    command = Serial.read();
    #if DEBUG
    Serial.print("Got command: ");
    Serial.println((int)command);
    #endif
    radio.stopListening();
    
    bool success;
    int attempts = 0;
    do {
      #if DEBUG
      Serial.println("attempting transmission");
      #endif
      success = radio.write(&command, 1);
      attempts += 1;
    } while (!success && attempts < RETRANS_LIMIT);
    //radio.writeBlocking(&command, 1, 500);
    //radio.txStandBy(500);

    if (!success) {
      #if DEBUG
      Serial.println("Giving up");
      #endif
    }
    else {
      #if DEBUG
      Serial.println("Sent command");
      #endif
      
      if (0b00010000 & command) {
        Serial.readBytes(ori, 2*sizeof(float));

        radio.writeBlocking(ori, 2*sizeof(float), 500);
        radio.txStandBy(500);
        #if DEBUG
        Serial.println("sent orientation");
        #endif
      }

      radio.startListening();

      length = getResponseSize(command);
      char buffer[length];

      #if DEBUG
      Serial.println("waiting for response");
      #endif

      for (int i = 0; i < length; i += 32)
        radio.read(buffer+i, min(length-i, 32));

      #if DEBUG
      Serial.print("expecting length: ");
      Serial.println(length);
      Serial.print("got response: ");
      for (int i = 0; i + 3 < length; i = i + sizeof(float)) {
        Serial.print(*(float*)(buffer + i * sizeof(float)));
        Serial.print(" ");
      }
      Serial.println("");
      #else
      Serial.write(buffer, length);
      #endif
    }
    radio.flush_rx();
    radio.flush_tx();
  }
  delay(15);
}

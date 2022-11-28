// This file provides an I2C interface with the HC-SR04 throught the bit-banged interface on the PIC12F675

#include <Wire.h>

#define HC_SR04_DEFAULT_ADDRESS 0x2B
#define PULSE_TO_DIST 0

void initialize() {
    Wire.begin();
}

float pulse_to_dist (short pulses) {
    return pulses*PULSE_TO_DIST;
}

short request(int address) {
    Wire.setClock(100000);
    Wire.requestFrom(address, 2, true);

    short pulses = Wire.read() << 8;
    pulses += Wire.read();

    return pulses;
}

float requestDistance(int address = HC_SR04_DEFAULT_ADDRESS) {
    return pulse_to_dist(request(address));
}
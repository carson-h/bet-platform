# BET Platform

This repository contains the code and documentation for the aerial and ground platforms of the BET project.

# Hardware

The aerial platform uses an Arduino UNO R3 to as the MCU and core of the platform. Through a custom soldered shield, it interfaces with the motors, comms. board, and sensor platform. Two 9g servo motors are used to actuate and stabilize the sensor platform. Communication with the ground station is accomplished with a pair of nRF24L01+ 2.4GHz trancievers. The sensor platform is fitted with 3 sensors and board and PIC microcontroller to interface all of them with the Arduino. These are: the AMG8833, a thermal sensor array; the LSM6DS3TR-C, a accelerometer & gyroscope; and the HC-SR04, an ultrasonic distance sensor.

To learn more, see [Hardware](hardware.md).

# Usage

Compile this code for the desired target (Arduino Uno) and upload it to the device. Use of Arduino IDE is recommended for this. Ensure peripheral devices are connected matching the configuration noted in the program. Operate the accopanying ground station to utilize the device.

# Additional Documentation

[Wireless Commmunication Protocol](wireless.md)

Documentation for the specific hardware used in this project is provided freely by the manufacturer online.

* [Arduino Uno R3](https://docs.arduino.cc/static/9c741aca36170f5f51ff4753af3821b6/A000066-datasheet.pdf)
* [HC-SR04](https://drive.google.com/file/d/1EVeVC9sq9LfeJpi6mkSZEUftg7otLQPN/view)
* [AMG8833](https://learn.adafruit.com/adafruit-amg8833-8x8-thermal-camera-sensor)
* [LSM6DS3TR-C](https://learn.adafruit.com/adafruit-lsm6ds3tr-c-6-dof-accel-gyro-imu)
* [PIC12F675](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/41190G.pdf)
* [nRF24L01+](https://nrf24.github.io/RF24/)


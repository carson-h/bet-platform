# BET-Aerial Platform

This repository contains the code and documentation for the aerial platform of the BET project.

# Hardware

The platform uses an Arduino UNO R3 to as the MCU and core of the platform. Through a custom soldered shield, it interfaces with the motors, comms. board, and sensor platform. Two 9g servo motors are used to actuate and stabilize the sensor platform. Communication with the ground station is accomplished with a pair of nRF24L01+ 2.4GHz trancievers. The sensor platform is fitted with 3 sensors and board and PIC microcontroller to interface all of them with the Arduino. These are: the AMG8833, a thermal sensor array; the LSM6DS3TR-C, a accelerometer & gyroscope; and the HC-SR04, an ultrasonic distance sensor.

# Usage

Compile this code for the desired target and upload it to the device. Use of Arduino IDE is recommended for this. Ensure peripheral devices are connected matching the configuration noted in the program. Operate the accopanying ground station to utilize the device.

# Documentation

[Wireless Commmunication Protocol](wireless.md)

More documentation for the hardware used in this project is provided freely by the manufacturer online.

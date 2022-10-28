# Hardware

This document details the hardware configuration of the aerial platform.

# Controller

The controller used for the project is the Arduino Uno R3. The board provides power and control for the rest of the BET-Aerial platform. A shield is used to provide electrical data and power connection points for all peripheral devices. Power is provided by an attached 9V battery through the boards internal power regulation.

## Pin Configuration

| Pin | Signal | Description                       |
|-----|--------|-----------------------------------|
| A4  | SDA    | I2C Bus Data for sensor platform  |
| A5  | SCL    | I2C Bus Clock for sensor platform |
| D5  | SERVO1 | PWM Control signal for servo 1    |
| D6  | SERVO2 | PWM Control signal for servo 2    |
| D7  | CE     | Chip enable for tranceiver        |
| D8  | CSN    | Chip select for tranceiver        |
| D11 | MOSI   | SPI MOSI for tranceiver           |
| D12 | MISO   | SPI MISO for tranceiver           |
| D13 | SCK    | SPI SCK for tranceiver            |

# HC-SR04 I2C Interface

The HC-SR04 is the only sensor on the sensor platform which does not provide an I2C communications interface. To minimize the number of connections required, an interface was developed to provide an I2C connection and probe the sensor when requested using a PIC12 microporcessor. This device lives on a board connecting all the sensors together on a common I2C bus.

## Pin Configuration

| Pin   | Signal | Description                                          |
|-------|--------|------------------------------------------------------|
| GPIO1 | TRIG   | Triggers the HR-SR04 to begin ultasonic measurements |
| GPIO2 | ECHO   | Pulse response from the HR-SR04                      |
| GPIO4 | SDA    | I2C Bus Data                                         |
| GPIO5 | SCL    | I2C Bus Clock                                        |

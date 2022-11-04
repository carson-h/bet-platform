# Ground Hardware

This document details the hardware configuration of the ground platform.

# Controller: NodeMCU ESP-12E

An Espressif ESP8266 is used as the MCU for the ground station. It is the core of the NodeMCU ESP-12E controller. Power is provided through the USB Micro Type B port. This interface also supplies a serial connection to the device from a host computer for data handling and control.

## Pin Configuration

Pin positions are provided both for reference on the board, and for the internal reference connections when programming the device.

| Board Pin | Internal Pin | Signal | Description                    |
|-----------|--------------|--------|--------------------------------|
| D1        | GPIO5        | CSN    | SPI chip select for tranceiver |
| D2        | GPIO4        | CE     | Chip enable for tranceiver     |
| D5        | GPIO14/HSCLK | CLK    | SPI clock for tranceiver       |
| D6        | GPIO12/HMISO | MISO   | SPI MISO for tranceiver        |
| D7        | GPIO13/HMOSI | MOSI   | SPI MOSI for tranceiver        |

# Wireless Communication: nRF24L01+

While this module does include integrated wireless communication, it was selected after the wireless communication hardware had already been determined. The same nRF24L01+ module used in the aerial platform was used for the ground station.

## Pin Configuration

Pin positions are provided both for reference on the board, and for the internal reference connections when programming the device. The pins of the device are enumerate as shown in the image below, with the darkened square representing the boxed in ground pin on the pcb of the module.

<center>
<img src="images/nRF24pins.png">
</center>

| Pin | Signal | Description                    |
|-----|--------|--------------------------------|
| 1   | GND    | Ground                         |
| 2   | VCC    | 3.3V Power                     |
| 3   | CE     | Chip enable for tranceiver     |
| 4   | CSN    | SPI chip select for tranceiver |
| 5   | CLK    | SPI clock for tranceiver       |
| 6   | MOSI   | SPI MOSI for tranceiver        |
| 7   | MISO   | SPI MISO for tranceiver        |

# Leffell High Altitude Balloon Main Arduino Code

This project is in the process of being assembled. Code from other components can be found elsewhere in the [Leffell-Space](https://github.com/orgs/Leffell-Space/repositories) organization, and we are adding code from those separate repositories to this one. This will be the final code that gets launched.

## Features

- Collects temperature, humidity, and pressure data
- Tracks balloon location using GPS
- Records high-quality flight footage
- Transmits data wirelessly to a ground station

## Components


| Component | Pin(s) | Mass(g) |         
| - | - | - |
|Weather Balloon| N/A|200|
|48-inch Parachute| N/A|N/A|
|15in x 13in x 10in Styrofoam Box (Stores Components)| N/A|366|
|SD Card Reader|MISO - 50, MOSI - 51, CLK - 52, CS - 53|4.5|
|DS18B20 Internal Temperature Sensor (Green Tape)| 5 | 9.1 |
|DS18B20 External Temperature Sensor | 6 | 9.1 |
|GY-GPS6MV2 GPS Module| RX1, TX1 |74|
|MS5611-01BA03 Barometric Pressure Sensor| A4,A5 (0x76,77)|2|
|I2C Ozone Sensor| A4,A5 (0x73) |5|
|Sensirion I2C SCD30 - Carbon Dioxide Sensor|A4, A5 (0x61)|3|
|MG-811 Analog CO2 Gas Sensor| A0 |6|
|Buzzer| 4 |7.7|
|Insta360 ONE X2 Camera|N/A|223|
|TalentCell Lithium ion battery pack|Arduino and Camera|529|
|Apple Airtag | N/A (Stand alone)|11|

- Arduino Mega needs to be used as Arduino UNO does not have enough storage space
- Wireless transmitter [Add more info]
- SD Card [Also add more info]
- Data that will be received will be displayed with these column headers
- Time, Latitude, Longitude, Altitude, HDOP, Inside Temperature, Outside Temperature, Pressure, Ozone Concentration, CO2 Concentration, Temperature, Humidity
- [Add other components]

## Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/Leffell-Space/Arduino_Main_Weather_Balloon.git
   ```
2. Open the project in the Arduino IDE
3. Install the necessary libraries
4. **Configure your build options:**  
   Copy the `config.h.example` file to `config.h` in the same directory. Edit `config.h` to enable or disable features (such as sensors and debug output) by changing the values from `1` (enabled) to `0` (disabled) as needed for your hardware setup.
   ```bash
   cp src/config.h.example src/config.h
   ```
5. Upload the code to your Arduino board

## Leffell Space Program Members
- Raz Idan (Instructor)
- Spencer Kolodny (JR)
- Ari Bellman (SO)
- Daniel Lerner (SO)
- Ari Messinger (SO)
- Oren Podietz (SO)
  
## [Add finished project documentation]

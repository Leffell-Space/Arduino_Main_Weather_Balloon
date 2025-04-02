# Leffell High Altitude Balloon Main Arduino Code

This project is currently in the process of being assembled. Other code from other components can be found eleswhere in the [Leffell-Space](https://github.com/orgs/Leffell-Space/repositories) organization and we are in the process of adding code from those separate repositories to this one. This is will be the final code that gets launched.

## Features

- Collects temperature, humidity, and pressure data
- Tracks balloon location using GPS
- Transmits data wirelessly to a ground station

## Components


| Component | Pin(s)/Arduino  | Mass(g) | Power Draw |           
| - | - | - | - |
|Weather Balloon| N/A|200|N/A|
|48-inch Parachute| N/A| N/A|N/A|
|15in x 13in x 10in Styrofoam Box (Stores Components)| N/A|366|N/A|
|DS18B20 Internal Temperature Sensor (Green Tape)| 5 / Master Arduino| 9.1|N/A|
|DS18B20 External Temperature Sensor | 6 / Master Arduino| 9.1 |N/A|
|MS5611-01BA03 Barometric Pressure Sensor| A4,A5 (0x76,77) / Slave Arduino|2| N/A|
|GY-GPS6MV2 GPS Module| 3,4 / Master Arduino|74|N/A|
|I2C Ozone Sensor| A4,A5 (0x73) / Slave Arduino|5| N/A |
|Sensirion I2C SCD30 - Carbon Dioxide Sensor|SCL, SDA (0x61) / Slave Arduino|3|N/A|
|EXCEED-RC 1800 mAh battery| Master and Slave Arduino|273| |
|AKASO EK7000 4K Camera (x2)|N/A (Stand alone)|288|N/A|
|Apple Airtag | N/A (Stand alone)|11|N/A|

- Wireless transmitter [Add more info]
- Data that will be received will be displayed with these column headers
     - Time, Latitude, Longitude, Altitude, Satellite Count, HDOP, Inside Temperature, Outside Temperature
- [Add other components]

## Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/Leffell-Space/Arduino_Main_Weather_Balloon.git
   ```
2. Open the project in the Arduino IDE
3. Install the necessary libraries
4. Upload the code to your Arduino board

## Leffell Space Program Members
- Raz Idan (Instructor)
- Spencer Kolodny (JR)
- Ari Bellman (SO)
- Daniel Lerner (SO)
- Ari Messinger (SO)
- Oren Podietz (SO)
  
## [Add license and link to license]

## [Add finished project documentation]

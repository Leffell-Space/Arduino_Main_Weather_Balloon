# Leffell High Altitude Balloon Main Arduino Code

This project is currently in the process of being assembled. Other code from other components can be found eleswhere in the [Leffell-Space](https://github.com/orgs/Leffell-Space/repositories) organization and we are in the process of adding code from those separate repositories to this one. This is will be the final code that gets launched.

## Features

- Collects temperature, humidity, and pressure data
- Tracks balloon location using GPS
- Transmits data wirelessly to a ground station

## Components

- 200g Weather Balloon
- 53-inch Parachute (TBD at a later time)
- 15in x 13in x 10in Styrofoam Box (Stores Components)
- Master Arduino Uno
- Slave Arduino Uno
  

| Device | Pin(s)/Arduino  | Mass(g) | Power Draw |           
| - | - | - | - |
|DS18B20 Internal Temperature Sensor (Green Tape)| 5 / Master Arduino| 9.1| |
|DS18B20 External Temperature Sensor | 6 / Master Arduino| 9.1 | |
|MS5611-01BA03 Barometric Pressure Sensor| A4,A5 / Slave Arduino|2| |
|GY-GPS6MV2 GPS Module| 3,4 / Master Arduino|74| |
|I2C Ozone Sensor| A4,A5 / Slave Arduino|5| |
|MG-811 Analog CO2 Gas Sensor| A0 / Slave Arduino|6| |
|EXCEED-RC 1800 mAh battery| Master and Slave Arduino|273| |
|AKASO EK7000 4K Camera (x2)| N/A|288| |
|Apple Airtag | N/A|11| |


- Wireless transmitter [Add more info]
- [Add other components]

## Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/Leffell-Space/Arduino_Main_Weather_Balloon.git
   ```
2. Open the project in the Arduino IDE
3. Install the necessary libraries (if any)
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

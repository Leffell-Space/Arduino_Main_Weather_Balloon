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
  

| Sensor | Port/Arduino  | Weight | Power Draw |           
| -  | - | - | - |
|January| $250    | 500| |
| February | $80     | 300 | |
| March    | $420    | 200| |

- DS18B20 internal temperature sensor attached to pin 5 (Green Tape)
- DS18B20 external temperature sensor attached to pin 6
- MS5611-01BA03 Barometric Pressure sensor
- GY-GPS6MV2 GPS module attached to pin 3 and 4
- Wireless transmitter [Add more info]
- Power supply (EXCEED-RC 1800 mAh battery)
- Apple Airtag 
- AKASO EK7000 4K Camera (x2)
- I2C Ozone Sensor 
- MG-811 Analog CO2 Gas Sensor
- PM2.5 Air Quality Sensor (Particulate Sensor)
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

#include <MS5611.h>
#include "DFRobot_OzoneSensor.h"

MS5611 baro;
int32_t pressure;
float filtered = 0;
int sensorValue = 0;
const int AQPin = 0;
float volts;

const int R0 = 346.76;
const int R2 = 1965;
float RS;
float PPM_acetone;

void setup() {
  // Start barometer
  baro = MS5611();
  baro.begin();
  // Start serial (UART)
  Serial.begin(9600);
  delay(2);
}

void loop() {
  // Read pressure
  pressure = baro.getPressure();
  sensorValue = analogRead(AQPin); 

  volts = sensorValue * 5;
  volts = volts / 1023;  
  
  if(filtered != 0){
    filtered = filtered + 0.1*(pressure-filtered);
  } else {
    filtered = pressure;          // first reading so set filtered to reading
  }

  // calculate RS
   RS = R2 * (1-volts);
   RS = RS/volts;
   // calculate acetone PPM
   PPM_acetone = 159.6 - 133.33*(RS/R0);
   // print out the acetone concentration:

  // Send pressure via serial (UART);
  Serial.print("Pressure: " + String(filtered) + " | ");
  Serial.print("Air Quality: " + String(PPM_acetone) + " | ");
  Serial.println("Volts: " + String(volts));

  delay(1);
}
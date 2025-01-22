#include <MS5611.h>

MS5611 baro;
int32_t pressure;
float filtered = 0;

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
  
  if(filtered != 0){
    filtered = filtered + 0.1*(pressure-filtered);
  }
  else {
    filtered = pressure;          // first reading so set filtered to reading
  }
  // Send pressure via serial (UART);
  Serial.println("Pressure: " + String(filtered));
}
#include <MS5611.h>
#include "DFRobot_OzoneSensor.h"
#include <SensirionI2cScd30.h>
#include <Wire.h>

#define COLLECT_NUMBER 20  // collect number, the collection range is 1-100
#define Ozone_IICAddress OZONE_ADDRESS_3
#define N_FLOATS 4

volatile byte* arrayPointer;
float array[N_FLOATS] = {0, 0, 0, 0};

#define BUZZER_PIN 9  // Define buzzer pin

DFRobot_OzoneSensor Ozone;

MS5611 baro;
int32_t pressure;
float filtered = 0;
int32_t sensorValue = 0;
const int AQPin = 0;
float volts;


void I2C_TxHandler(void)
{
  byte buffer[4*N_FLOATS];
  arrayPointer = (byte*) &array;
  for(byte i = 0; i < 4*N_FLOATS; i++) 
    buffer[i] = arrayPointer[i];
  Wire.write(buffer,4*N_FLOATS);
}

void setup() {
  // Start barometer
  baro = MS5611();
  baro.begin();
  // Start serial (UART)
  Serial.begin(9600);
  if (!Ozone.begin(Ozone_IICAddress)) {
    Serial.println("Ozone sensor I2c device number error !");
  } else {
    Serial.println("Ozone sensor working");
  }

  /**
   * set measuer mode
   * MEASURE_MODE_AUTOMATIC         active  mode
   * MEASURE_MODE_PASSIVE           passive mode
   */
  Ozone.setModes(MEASURE_MODE_PASSIVE);

    Wire.begin(0x55); // Initialize I2C (Slave Mode: address=0x55 )
    Wire.onRequest(I2C_TxHandler);

  pinMode(BUZZER_PIN, OUTPUT);

  Wire.begin();
  sensor.begin(Wire, SCD30_I2C_ADDR_61);
  sensor.startPeriodicMeasurement(0);

  delay(2);
}

void loop() {
  // Read pressure
  pressure = baro.getPressure();

  if (filtered != 0) {
    filtered = filtered + 0.1 * (pressure - filtered);
  } else {
    filtered = pressure;  // first reading so set filtered to reading
  }

  int16_t ozoneConcentration = Ozone.readOzoneData(COLLECT_NUMBER);

  // Calculate altitude (assuming sea level pressure is 1013.25 hPa)
  float altitude = 44330.0 * (1.0 - pow(filtered / 101325.0, 0.1903));

  sensor.blockingReadMeasurementData(co2Concentration, temperature, humidity);

  Serial.print("Pressure: " + String(filtered) + " | ");
  Serial.print("Altitude: " + String(altitude) + " ft | ");
  Serial.print("Ozone: " + String(ozoneConcentration) + " PPB  |");
  Serial.print("CO2 Concentration: " + String(co2Concentration) + " ppm | ");
  Serial.print("Temperature: " + String(temperature) + " °C | ");
  Serial.println("Humidity: " + String(humidity) + " RH%.");
  if (altitude < 300) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(1);
}
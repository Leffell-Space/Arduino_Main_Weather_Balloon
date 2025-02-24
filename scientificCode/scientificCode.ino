#include <MS5611.h>
#include "DFRobot_OzoneSensor.h"

#define COLLECT_NUMBER  20              // collect number, the collection range is 1-100
#define Ozone_IICAddress OZONE_ADDRESS_3
#define N_FLOATS 4

volatile byte* arrayPointer;
float array[N_FLOATS] = {0, 0, 0, 0};

DFRobot_OzoneSensor Ozone;

MS5611 baro;
int32_t pressure;
float filtered = 0;
int32_t sensorValue = 0;
const int AQPin = 0;
float volts;

const int R0 = 346.76;
const int R2 = 1965;
float RS;
float PPM_acetone;
float ozoneConcentration;

byte TxByte = 0;
 
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
  if(!Ozone.begin(Ozone_IICAddress)){
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

   //Get the ozone concentration
   ozoneConcentration = Ozone.readOzoneData(COLLECT_NUMBER);

   array[0] = filtered;
   array[1] = PPM_acetone;
   array[2] = volts;
   array[3] = ozoneConcentration;

  // Send pressure via serial (UART);
   //Serial.print("Pressure: " + String(filtered) + " | "); //32 bits
   //Serial.print("Air Quality: " + String(PPM_acetone) + " | "); //16 bits
   //Serial.print("Volts: " + String(volts) + " | "); //16 bits
   //Serial.println("Ozone: " + String(ozoneConcentration) + " PPB."); //16 bits
   //delay(1000);
}
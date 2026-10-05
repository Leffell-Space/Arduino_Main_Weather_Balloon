#include <SD.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <MS5611.h>
#include "DFRobot_OzoneSensor.h"
#include <SensirionI2cScd30.h>
#include "config.h"
#include <math.h>
TinyGPSPlus gps;

#if enable_TempSensors
#define INSIDE0 3   //inside temps
#define INSIDE1 4
#define OUTSIDE0 5  //outside temps
#define OUTSIDE1 6

// Setup a oneWire instance to communicate with any OneWire devices
OneWire in0(INSIDE0);
OneWire in1(INSIDE1);
OneWire out0(OUTSIDE0);
OneWire out1(OUTSIDE1);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors_in0(&in0);
DallasTemperature sensors_in1(&in1);
DallasTemperature sensors_out0(&out0);
DallasTemperature sensors_out1(&out1);
#endif

File myFile;

String dataFile = "data.csv";

#if enable_Ozone
#define COLLECT_NUMBER 20  // collect number, the collection range is 1-100
#define Ozone_IICAddress OZONE_ADDRESS_3
#endif

#if enable_Sensirion
SensirionI2cScd30 sensor;
#endif


#if enable_Ozone
DFRobot_OzoneSensor Ozone;
#endif

#if enable_MS5611
MS5611 baro;
#endif

float pressure = 0;
int16_t ozoneConcentration = 0;
float filtered = 0;
float co2Concentration = 0;
float temperature = 0;
float humidity = 0;
float insideCelsius0 = 0.0;
float insideCelsius1 = 0.0;
float outsideCelsius0 = 0.0;
float outsideCelsius1 = 0.0;


#define OZONE_STALE_VALUE INT16_MIN

//need to calibrate before launch/assembly
float insideOffset0 = 0;
float insideOffset1 = 0;
float outsideOffset0 = 0;
float outsideOffset1 = 0;

double latitude = 0.0;
double longitude = 0.0;
double altitude = 0.0;
double hdop = 0.0;
int hours = 0;
int minutes = 0;
int seconds = 0;
unsigned long previousMillis = 0;
unsigned long lastGPSRead = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);  // Try different baud rate

// SD Card Initialization
#if debug
  if (!SD.begin(53)) {
    Serial.println("SD initialization failed!");
  } else {
    Serial.println("SD initialized successfully");
  }
#else
  SD.begin(53);
#endif

#if debug
  if (SD.exists(dataFile)) {
    Serial.println("File exists");
  } else {
    Serial.println("Creating file");
  }
#endif

  // Create/Open file
  myFile = SD.open(dataFile, FILE_WRITE);
  if (myFile) {
    myFile.println("Time,Lat,Long,Alt,HDOP,Inside0,Inside1,Outside0,Outside1,Pressure,Ozone,CO2,Temperature,Humidity");
    myFile.flush();
    myFile.close();
#if debug
    Serial.println("Header written to file");
#endif
  } else {
#if debug
    Serial.println("Error opening file");
#endif
  }

  // Start barometer
  Wire.begin();

#if enable_MS5611
  baro = MS5611();
  baro.begin();
#endif

#if debug && enable_Ozone
  if (!Ozone.begin(Ozone_IICAddress)) {
    Serial.println("Ozone sensor I2c device number error!");
  } else {
    Serial.println("Ozone sensor working");
  }
#endif
#if enable_Ozone
  Ozone.begin(Ozone_IICAddress);
  Ozone.setModes(MEASURE_MODE_PASSIVE);
#endif

// Start up the temperature sensors
#if enable_TempSensors
  sensors_in0.begin();
  sensors_in1.begin();
  sensors_out0.begin();
  sensors_out1.begin();
#endif


#if enable_Sensirion
  sensor.begin(Wire, SCD30_I2C_ADDR_61);
  sensor.startPeriodicMeasurement(0);
#endif
}

void loop() {
  // Process GPS data
  unsigned long currentMillis = millis();

  // Continuously feed GPS data
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }

// Check GPS status every 2 seconds
#if wokwi_test
  unsigned long gps_time = 1000;
#else
  unsigned long gps_time = 2000;
#endif
  if (currentMillis - lastGPSRead >= gps_time) {
    lastGPSRead = currentMillis;

    if (gps.location.isValid()) {
      // Get location information
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      altitude = gps.altitude.meters();  // Altitude in meters
      hdop = gps.hdop.hdop();            // Horizontal dilution of precision

      // Get the timestamp (in hours, minutes, seconds)
      hours = gps.time.hour();
      minutes = gps.time.minute();
      seconds = gps.time.second();
    }
  }

// Read other sensors and process data every 10 seconds
#if wokwi_test
  unsigned long process_time = 1000;
#else
  unsigned long process_time = 5000;
#endif
  if (currentMillis - previousMillis >= process_time) {
    previousMillis = currentMillis;

// Read pressure
#if enable_MS5611
    baro.read();
    pressure = baro.getPressure();
#endif

#if enable_Ozone
    ozoneConcentration = Ozone.readOzoneData(COLLECT_NUMBER);
#endif

#if enable_Sensirion
    sensor.blockingReadMeasurementData(co2Concentration, temperature, humidity);
#endif

#if enable_TempSensors
    sensors_in0.requestTemperatures();
    sensors_in1.requestTemperatures();
    sensors_out0.requestTemperatures();
    sensors_out1.requestTemperatures();

    insideCelsius0 = sensors_in0.getTempCByIndex(0) + insideOffset0;
    insideCelsius1 = sensors_in1.getTempCByIndex(0) + insideOffset1;
    outsideCelsius0 = sensors_out0.getTempCByIndex(0) + outsideOffset0;
    outsideCelsius1 = sensors_out1.getTempCByIndex(0) + outsideOffset1;
#endif



    // Format and write data to SD
    String timeStr = String(hours < 10 ? "0" : "") + String(hours) + ":" + String(minutes < 10 ? "0" : "") + String(minutes) + ":" + String(seconds < 10 ? "0" : "") + String(seconds);

    String dataStr = timeStr + "," + String(latitude, 6) + "," + String(longitude, 6) + "," + String(altitude) + "," + String(hdop) + "," +   //gps
    String(insideCelsius0) + "," + String(insideCelsius1) + "," + String(outsideCelsius0) + "," + String(outsideCelsius1) + "," +             //temps
    String(pressure) + "," + String(ozoneConcentration) + "," + String(co2Concentration) + "," + String(temperature) + "," + String(humidity); //environmental

    myFile = SD.open(dataFile, FILE_WRITE);
    if (myFile) {
#if debug
      Serial.println("Writing to SD: " + dataStr);
#endif
      myFile.println(dataStr);
      myFile.close();
    } else {
#if debug
      Serial.println("Error opening file for writing");
#endif
    }
    pressure = NAN;
#if enable_Sensirion
    co2Concentration = NAN;
    temperature = NAN;
    humidity = NAN;
#endif
    insideCelsius0 = NAN;
    insideCelsius1 = NAN;
    outsideCelsius0 = NAN;
    outsideCelsius1 = NAN;

    ozoneConcentration = OZONE_STALE_VALUE;
  }
}

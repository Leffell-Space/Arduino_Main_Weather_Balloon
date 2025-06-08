#include <SD.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <MS5611.h>
#include "DFRobot_OzoneSensor.h"
#include <SensirionI2cScd30.h>
#include <config.h>

TinyGPSPlus gps;

#define INSIDE 5   //inside temp
#define OUTSIDE 6  //outside temp

// Setup a oneWire instance to communicate with any OneWire devices
OneWire in(INSIDE);
OneWire out(OUTSIDE);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors_in(&in);
DallasTemperature sensors_out(&out);

File myFile;

String dataFile = "data.csv";

#define COLLECT_NUMBER 20  // collect number, the collection range is 1-100
#define Ozone_IICAddress OZONE_ADDRESS_3
SensirionI2cScd30 sensor;

#define BUZZER_PIN 4  // Define buzzer pin

DFRobot_OzoneSensor Ozone;

MS5611 baro;
int32_t pressure;
float filtered = 0;
float co2Concentration = 0;
float temperature = 0;
float humidity = 0;

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

  if (SD.exists(dataFile)) {
    Serial.println("File exists");
  } else {
    Serial.println("Creating file");
  }

  // Create/Open file
  myFile = SD.open(dataFile, FILE_WRITE);
  if (myFile) {
    myFile.println("Time,Lat,Long,Alt,HDOP,Inside Temp,Outside Temp,Pressure,Ozone,CO2,Temperature,Humidity");
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
  baro = MS5611();
  baro.begin();
  #if debug
    if (!Ozone.begin(Ozone_IICAddress)) {
      Serial.println("Ozone sensor I2c device number error!");
    } else {
      Serial.println("Ozone sensor working");
    }
  #else
    Ozone.begin(Ozone_IICAddress);
  #endif
  Ozone.setModes(MEASURE_MODE_PASSIVE);
  
  // Start up the temperature sensors
  sensors_in.begin();
  sensors_out.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  sensor.begin(Wire, SCD30_I2C_ADDR_61);
  sensor.startPeriodicMeasurement(0);  
}

void loop() {
  // Process GPS data
  unsigned long currentMillis = millis();
  
  // Continuously feed GPS data
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }
  
  // Check GPS status every 5 seconds
  if (currentMillis - lastGPSRead >= 2000) {
    lastGPSRead = currentMillis;
    
    if (gps.location.isValid()) {
      // Get location information
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      altitude = gps.altitude.meters();  // Altitude in meters
      hdop = gps.hdop.hdop();  // Horizontal dilution of precision

      // Get the timestamp (in hours, minutes, seconds)
      hours = gps.time.hour();
      minutes = gps.time.minute();
      seconds = gps.time.second();
    }
  }

  // Read other sensors and process data every 10 seconds
  if (currentMillis - previousMillis >= 10000) {
    previousMillis = currentMillis;
    
    // Read pressure
    pressure = baro.getPressure();

    int16_t ozoneConcentration = Ozone.readOzoneData(COLLECT_NUMBER);
    sensor.blockingReadMeasurementData(co2Concentration, temperature, humidity);

    sensors_in.requestTemperatures();
    float insideCelsius = sensors_in.getTempCByIndex(0);
    sensors_out.requestTemperatures();
    float outsideCelsius = sensors_out.getTempCByIndex(0);

    if (altitude < 300) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }

    // Format and write data to SD
    String timeStr = String(hours < 10 ? "0" : "") + String(hours) + ":" + 
                    String(minutes < 10 ? "0" : "") + String(minutes) + ":" + 
                    String(seconds < 10 ? "0" : "") + String(seconds);
                    
    String dataStr = timeStr + "," + 
                    String(latitude, 6) + "," + 
                    String(longitude, 6) + "," + 
                    String(altitude) + "," + 
                    String(hdop) + "," +
                    String(insideCelsius) + "," + 
                    String(outsideCelsius) + "," + 
                    String(pressure) + "," + 
                    String(ozoneConcentration) + "," + 
                    String(co2Concentration) + "," + 
                    String(temperature) + "," + 
                    String(humidity);
    
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
  }
}

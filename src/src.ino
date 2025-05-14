#include <SD.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPS++.h>  // test
#include <SoftwareSerial.h>
#include <Wire.h>
#include <MS5611.h>
#include "DFRobot_OzoneSensor.h"
#include <SensirionI2cScd30.h>

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

#define BUZZER_PIN 9  // Define buzzer pin

DFRobot_OzoneSensor Ozone;

MS5611 baro;
int32_t pressure;
float filtered = 0;
float co2Concentration = 0;
float temperature = 0;
float humidity = 0;

double latitude;
double longitude;
double altitude;
int hours;
int minutes;
int seconds;
int previousMillis = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
    // SD Card Initialization
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
  }

  if (SD.exists(dataFile)) {
    SD.remove(dataFile);
    Serial.println("file exists");
  } else {
    Serial.println("Creating file");
  }

  // Create/Open file
  myFile = SD.open(dataFile, FILE_WRITE);
  myFile.println("Time,Lat,Long,Alt,Inside Temp,Outside Temp,Pressure,Ozone Concentration,CO2 Quality,Temperature,Humidity");
  myFile.flush();
  
  // Start barometer
  baro = MS5611();
  baro.begin();
  
  // Start serial (UART)

  if (!Ozone.begin(Ozone_IICAddress)) {
    Serial.println("Ozone sensor I2c device number error !");
  } else {
    Serial.println("Ozone sensor working");
  }
  Ozone.setModes(MEASURE_MODE_PASSIVE);
  

  // Start up the library
  sensors_in.begin();
  sensors_out.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  sensor.begin(Wire, SCD30_I2C_ADDR_61);
  sensor.startPeriodicMeasurement(0);
}

void loop() {
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());  // Feed data to the GPS library
    //Serial.println(gps.location.isUpdated());
    if (gps.location.isUpdated()) {
      //Serial.println("BREAK 3");

      // Get location information
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      altitude = gps.altitude.meters();  // Altitude in meters

      // Get the timestamp (in hours, minutes, seconds)
      hours = gps.time.hour();
      minutes = gps.time.minute();
      seconds = gps.time.second();
    }
  }

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

    float data[13] = {hours, minutes, seconds, latitude, longitude, altitude, insideCelsius, outsideCelsius, pressure, ozoneConcentration, co2Concentration, temperature, humidity};

    int currentMillis = millis();
    if (currentMillis - previousMillis >= 10000) {  //write to SD card only every 10 seconds
      previousMillis = currentMillis;
      myFile = SD.open(dataFile, FILE_WRITE);
      if (myFile) {
        // use a for loop to concatante the array into a string
        String allData = "";        //write one long string
        for (int i = 0; i < 13; i++) {
          if (i < 2) {
            allData += String(data[i]) + ":";
          } else if (i == 12) {
            allData += String(data[i]);
          } else {
            allData += String(data[i]) + ",";
          }
        }
        Serial.println(allData);
        myFile.println(allData);
        myFile.flush();
        myFile.close();
      }
    }
}

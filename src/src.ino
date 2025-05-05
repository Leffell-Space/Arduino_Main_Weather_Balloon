#include <SD.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPS++.h>  // test
#include <SoftwareSerial.h>
#include <Wire.h>
#include <MS5611.h>
#include "DFRobot_OzoneSensor.h"
//#include <SensirionI2cScd30.h>

TinyGPSPlus gps;

SoftwareSerial ss(4, 3);  // RX, TX pins (adjust as needed)

#define INSIDE 5
#define OUTSIDE 6

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
//SensirionI2cScd30 sensor;

#define BUZZER_PIN 9  // Define buzzer pin

//DFRobot_OzoneSensor Ozone;

MS5611 baro;
int32_t pressure;
float filtered = 0;
float co2Concentration = 0;
float temperature = 0;
float humidity = 0; 

void setup() {
  // Start barometer
  baro = MS5611();
  baro.begin();
  // Start serial (UART)
  Serial.begin(9600);
  /*
  if (!Ozone.begin(Ozone_IICAddress)) {
    Serial.println("Ozone sensor I2c device number error !");
  } else {
    Serial.println("Ozone sensor working");
  }
  */
  //Ozone.setModes(MEASURE_MODE_PASSIVE);

  ss.begin(9600);  // GPS baud rate

  // Start up the library
  sensors_in.begin();
  sensors_out.begin();
  
    pinMode(BUZZER_PIN, OUTPUT);
  //sensor.begin(Wire, SCD30_I2C_ADDR_61);
  //sensor.startPeriodicMeasurement(0);

  // SD Card Initialization
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  //Serial.println("initialization done.");

  if (SD.exists(dataFile)) {
    SD.remove(dataFile);
    Serial.println("file exists");
  } else {
    Serial.println("Creating file");
  }

  // Create/Open file
  myFile = SD.open(dataFile, FILE_WRITE);
  //Serial.println(myFile.position());
  myFile.println("Time,Latitude,Longitude,Altitude,Satellite Count,HDOP,Inside Temperature,Outside Temperature,Pressure,PPM Acetone,Air Quality,Ozone Concentration");
  //Serial.println(myFile.position());
  myFile.flush();
  //Serial.println("RAN");

  return;
}

void loop() {
  
  double latitude;
  double longitude;
  double altitude;
  int satelliteCount;
  int hours;
  int minutes;
  int seconds;
  float hdop;
  
    // Read pressure
  pressure = baro.getPressure();

  if (filtered != 0) {
    filtered = filtered + 0.1 * (pressure - filtered);
  } else {
    filtered = pressure;  // first reading so set filtered to reading
  }
  
  //int16_t ozoneConcentration = Ozone.readOzoneData(COLLECT_NUMBER);
  //sensor.blockingReadMeasurementData(co2Concentration, temperature, humidity);  

  if (altitude < 300) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
  
  //Serial.println("BREAK 1");
  while (ss.available() > 0) {
    gps.encode(ss.read());  // Feed data to the GPS library
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

      sensors_in.requestTemperatures();
      float insideCelsius = sensors_in.getTempCByIndex(0);
      sensors_out.requestTemperatures();
      float outsideCelsius = sensors_out.getTempCByIndex(0);

      // if the file opened okay, write to it:
      // Write to file
      myFile.print(hours);
      myFile.print(":");
      myFile.print(minutes);
      myFile.print(":");
      myFile.print(seconds);
      myFile.print(",");
      myFile.print(latitude, 6);
      myFile.print(",");
      myFile.print(longitude, 6);
      myFile.print(",");
      myFile.print(altitude, 3);
      myFile.print(",");
      myFile.print(insideCelsius);
      myFile.print(",");
      myFile.print(outsideCelsius);
      myFile.print(",");
      myFile.print(filtered);
      myFile.print(",");
      //myFile.print(ozoneConcentration);
      myFile.print(",");
      myFile.print(co2Concentration);
      myFile.print(",");
      myFile.print(temperature);
      myFile.print(",");
      myFile.print(humidity);
      myFile.print(",");
      myFile.flush();
      Serial.println(myFile.position());
      delay(1000);
      
    }
    
  }
  
}

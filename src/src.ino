#include <SD.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPS++.h>  // test
#include <SoftwareSerial.h>
#include <Wire.h>

TinyGPSPlus gps;

SoftwareSerial ss(9, 8);  // RX, TX pins (adjust as needed)

#define INSIDE 5
#define OUTSIDE 6

// Setup a oneWire instance to communicate with any OneWire devices
OneWire in(INSIDE);
OneWire out(OUTSIDE);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors_in(&in);
DallasTemperature sensors_out(&out);

const byte dataCount = 6;

union {
 float floatData[dataCount];
 byte rawData[dataCount*sizeof(float)];
}myData;

File myFile;

String dataFile = "data.csv";

void setup() {

  Serial.begin(9600);
  //ss.begin(9600);  // GPS baud rate

  // Start up the library
  //sensors_in.begin();
  //sensors_out.begin();

  // SD Card Initialization
  if (SD.begin(10)) {
    Serial.println("SD card is ready to use.");
  } else {
    Serial.println("SD card initialization failed");
    return;
  }

  if (SD.exists(dataFile)) {
    SD.remove(dataFile);
    Serial.println("file exists");
  } else {
    Serial.println("Creating file");
  }

  // Create/Open file
  myFile = SD.open(dataFile, FILE_WRITE);
  if (dataFile) {
  myFile.println("Time,Latitude,Longitude,Altitude,Satellite Count,HDOP,Inside Temperature,Outside Temperature,Pressure,PPM Acetone,Air Quality,Ozone Concentration");
  myFile.close();
  Serial.println("RAN");
  }

  return;
}

void displayData() {
  for (int i = 0; i < dataCount; i++)  {
    Serial.print ("Float n. ");
    Serial.print (i);
    Serial.print (" Value: ");
    Serial.println (myData.floatData[i],6);  
    } 
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
  
  Wire.requestFrom(0x55, 16); // Request From Slave @ 0x55, Data Length = 1Byte

  byte data;

  while(Wire.available()) {  // Read Received Data From Slave Device
    for(byte i = 0; i < 4*dataCount; i++)
      myData.rawData[i] = Wire.read(); 
    displayData();
  }
  
  Serial.println("BREAK 1");
  while (ss.available() > 0) {
    Serial.println("BREAK 2");
    gps.encode(ss.read());  // Feed data to the GPS library
    
    if (gps.location.isUpdated() && gps.satellites.isUpdated()) {
      // Get location information
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      altitude = gps.altitude.meters();  // Altitude in meters
  
      // Get the number of satellites in view
      satelliteCount = gps.satellites.value();

      // Get the timestamp (in hours, minutes, seconds)
      hours = gps.time.hour();
      minutes = gps.time.minute();
      seconds = gps.time.second();

      // Get the Horizontal Dilution of Precision (HDOP)
      hdop = gps.hdop.value() / 100.0;  // HDOP is returned in hundredths, so divide by 100

      sensors_in.requestTemperatures();
      float insideCelsius = sensors_in.getTempCByIndex(0);
      sensors_out.requestTemperatures();
      float outsideCelsius = sensors_out.getTempCByIndex(0);

      // if the file opened okay, write to it:
      
      if (myFile) {
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
        myFile.print(satelliteCount);
        myFile.print(",");
        myFile.print(hdop, 2);
        myFile.print(",");
        myFile.print(insideCelsius);
        myFile.print(",");
        myFile.print(outsideCelsius);
        for (int i = 0; i < dataCount; i++) {
          myFile.print(",");
          myFile.print(myData.floatData[i]);
        }
        myFile.println();
        myFile.flush();
      } else {  // if the file didn't open, print an error:
        Serial.println("error opening " + dataFile);
      }
      
    }
    
  }
  
}

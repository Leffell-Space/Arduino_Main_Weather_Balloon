#include <SD.h>
#include <SPI.h>
#include <DHT22.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPS++.h> // test
#include <SoftwareSerial.h>

TinyGPSPlus gps;

SoftwareSerial ss(4, 3); // RX, TX pins (adjust as needed)

// Data wire is conncted to the Arduino digital pin 4
#define INSIDE 5
#define OUTSIDE 6
#define pinDATA 7 // SDA, or almost any other I/O pin

// Setup a oneWire instance to communicate with any OneWire devices
OneWire in(INSIDE);
OneWire out(OUTSIDE);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors_in(&in);
DallasTemperature sensors_out(&out);

DHT22 dht22(pinDATA); 
 
File myFile;

int pinCS = 10; 

String dataFile = "data.csv";

void setup() {
    
  Serial.begin(9600);
  ss.begin(9600); // GPS baud rate

  pinMode(pinCS, OUTPUT);

  // Start up the library
  sensors_in.begin();
  sensors_out.begin();
  
  // SD Card Initialization
  if (SD.begin()) {
    Serial.println("SD card is ready to use.");
  } else {
    Serial.println("SD card initialization failed");
    return;
  }

  if(SD.exists(dataFile)) {
    SD.remove(dataFile);
  }
  
  // Create/Open file 
  myFile = SD.open(dataFile, FILE_WRITE);
  
  myFile.print("Time,Latitude,Longitude,Altitude,Satellite Count,HDOP,Humidity,Temperature,Inside Temperature,Outside Temperature");
  myFile.flush();
}

void loop() {
  double latitude;
  double longitude;
  double altitude;
  int satelliteCount;
  unsigned long timestamp;
  int hours;
  int minutes;
  int seconds;
  float hdop;

  if (ss.available() > 0) {
    gps.encode(ss.read()); // Feed data to the GPS library
   if (gps.location.isUpdated()) {
      // Get location information
      double latitude = gps.location.lat();
      double longitude = gps.location.lng();
      double altitude = gps.altitude.meters(); // Altitude in meters

      // Get the number of satellites in view
      int satelliteCount = gps.satellites.value();

      // Get the timestamp (in hours, minutes, seconds)
      unsigned long timestamp = gps.time.value();
      int hours = gps.time.hour();
      int minutes = gps.time.minute();
      int seconds = gps.time.second();

      // Get the Horizontal Dilution of Precision (HDOP)
      float hdop = gps.hdop.value() / 100.0; // HDOP is returned in hundredths, so divide by 100
   } 
  }
 
  float t = dht22.convertCtoF(dht22.getTemperature());
  float h = dht22.getHumidity();

  sensors_in.requestTemperatures();
  float insideCelsius = sensors_in.getTempCByIndex(0);
  delay(10);
  sensors_out.requestTemperatures();
  float outsideCelisus = sensors_out.getTempCByIndex(0);

    // if the file opened okay, write to it:
  if (myFile) {
    // Write to file
    if (gps.location.isUpdated()) { 
      myFile.print(hours); myFile.print(":");
      myFile.print(minutes); myFile.print(":");
      myFile.print(seconds); myFile.print(",");
      myFile.print(latitude, 6); myFile.print(",");
      myFile.print(longitude, 6); myFile.print(",");
      myFile.print(altitude, 1); myFile.print(",");
      myFile.print(satelliteCount); myFile.print(",");
      myFile.print(hdop, 2); myFile.print(",");
    }
    myFile.println();
    myFile.print(h, 1);
    myFile.print(",");
    myFile.print(t, 1);
    myFile.print(",");
    myFile.print(insideCelsius);
    myFile.print(",");
    myFile.print(outsideCelisus);
    myFile.flush();                                               
  } else { // if the file didn't open, print an error:
    Serial.println("error opening " + dataFile);
  }

  delay(2000); //Collecting period should be : >1.7 second
}

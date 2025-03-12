#include <SD.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPS++.h>  // test
#include <SoftwareSerial.h>

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

int pinCS = 10;

String dataFile = "data.csv";

void setup() {

  Serial.begin(9600);
  ss.begin(9600);  // GPS baud rate

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

  if (SD.exists(dataFile)) {
    SD.remove(dataFile);
  }

  // Create/Open file
  myFile = SD.open(dataFile, FILE_WRITE);

  Serial.println("Time,Latitude,Longitude,Altitude,Satellite Count,HDOP,Inside Temperature,Outside Temperature");
  myFile.flush();
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

  while (ss.available() > 0) {
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
        Serial.print(hours);
        Serial.print(":");
        Serial.print(minutes);
        Serial.print(":");
        Serial.print(seconds);
        Serial.print(",");
        Serial.print(latitude, 6);
        Serial.print(",");
        Serial.print(longitude, 6);
        Serial.print(",");
        Serial.print(altitude, 3);
        Serial.print(",");
        Serial.print(satelliteCount);
        Serial.print(",");
        Serial.print(hdop, 2);
        Serial.print(",");
        Serial.print(insideCelsius);
        Serial.print(",");
        Serial.print(outsideCelsius);
        Serial.println();
        myFile.flush();
      } else {  // if the file didn't open, print an error:
        Serial.println("error opening " + dataFile);
      }
    }
  }
}

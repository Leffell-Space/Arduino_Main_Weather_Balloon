#include <SD.h>
#include <SPI.h>
#include <DHT22.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is conncted to the Arduino digital pin 4
#define INSIDE 4
#define OUTSIDE 2
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

void setup() {
    
  Serial.begin(9600);
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

  if(SD.exists("test.txt")) {
    SD.remove("test.txt");
  }
  
  // Create/Open file 
  myFile = SD.open("test.txt", FILE_WRITE);
  
  myFile.print("Date | Time | Humidity | Temperature | Inside Temperature | Outside Temperature");
  myFile.flush();
}

void loop() {
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
    myFile.println();
    myFile.print(h, 1);
    myFile.print(" | ");
    myFile.print(t, 1);
    myFile.print(" | ");
    myFile.print(insideCelsius);
    myFile.print(" | ");
    myFile.print(outsideCelisus);
    myFile.flush();                                               
  } else { // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  delay(2000); //Collecting period should be : >1.7 second
}
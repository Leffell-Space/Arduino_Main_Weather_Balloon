#include <SD.h>
#include <SPI.h>
#include <DHT22.h>
//define pin data
#define pinDATA 7 // SDA, or almost any other I/O pin

DHT22 dht22(pinDATA); 
 
File myFile;

int pinCS = 10; 

void setup() {
    
  Serial.begin(9600);
  pinMode(pinCS, OUTPUT);
  
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
  
  myFile.print("Date | Time | Humidity | Temperature");
  myFile.flush();
}

void loop() {
  float t = dht22.convertCtoF(dht22.getTemperature());
  float h = dht22.getHumidity();

  if (dht22.getLastError() != dht22.OK) {
    Serial.print("last error :");
    Serial.println(dht22.getLastError());
  }

    // if the file opened okay, write to it:
  if (myFile) {
    // Write to file
    
    myFile.println();
    myFile.print(h, 1);
    myFile.print(" | ");
    myFile.print(t, 1);
    myFile.flush();                                               
  } else { // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  delay(2000); //Collecting period should be : >1.7 second
}
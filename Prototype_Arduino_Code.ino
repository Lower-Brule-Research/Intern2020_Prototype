/*------------------------------------------------------------------------------
This code get's loaded on to the Arduino and sends GPS location to an ESP8266 when a distress button is pressed.
If you don't have a GPS module connected, it will just send 0.00000 as the latitude and longitude.
------------------------------------------------------------------------------*/

//Libraries needed
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps; // The TinyGPS++ object
SoftwareSerial ss(RXPin, TXPin); // The serial connection to the GPS device
String gpslat;
String gpslong;

int DistressPin= 11;  // Connect the distress button data pin to digital pin 11 on the arduino

int DistressState = 0;

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  pinMode(DistressPin, INPUT);
}

void loop() 
{
  int DistressState = digitalRead(DistressPin);
  if (DistressState == 1) //If button gets pressed we send lat and long to ESP8266
    {  
      //Here GPS Data gets sent to the ESP8266 via a JSON response
      smartDelay(1000);
      gpslat = String(gps.location.lat(), 6);
      gpslong = String(gps.location.lng(), 6);
      DynamicJsonDocument doc(1024);
      doc["type"] = "response";
      doc["latitude"] = gpslat;
      doc["longitude"] = gpslong;
      serializeJson(doc,Serial);
      smartDelay(1000);
    }
}
static void smartDelay(unsigned long ms) 
  {
  unsigned long start = millis();
  do 
    {
      while (ss.available())
        gps.encode(ss.read());
    } 
  while (millis() - start < ms);
  }

/*------------------------------------------------------------------------------
This code get's loaded on to the Arduino and should send your GPS location to an ESP8266 when a distress button is pressed.
If you don't have a GPS module connected, it will just send 0.00000 as the latitude and longitude.
------------------------------------------------------------------------------*/
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>

SoftwareSerial gpsserial(4, 3); // The serial connection to the GPS device RXPin = 4, TXPin = 3;

int DistressPin= 11;  // Connect the distress button data pin to digital pin 11 on the arduino
int DistressState = 0;
String message = "";
bool messageReady = false;
String latitude = "Safe";
String longitude = "Safe";

void setup() {
  Serial.begin(9600);
  gpsserial.begin(9600);
  pinMode(DistressPin, INPUT);
}

void loop() {
  int DistressState = digitalRead(DistressPin);
  if (DistressState == 1) //If button gets pressed we save latitude and longitude
    {  
      TinyGPSPlus gps; // The TinyGPS++ object
      delay(1000);
      latitude = String(gps.location.lat(), 6);
      longitude = String(gps.location.lng(), 6);
    }
 
  while(Serial.available()) {
    message = Serial.readString();
    messageReady = true;
  }
  // Only process message if there's one
  if(messageReady) {
    // The only messages we'll parse will be formatted in JSON
    DynamicJsonDocument doc(1024); // ArduinoJson version 6+
    // Attempt to deserialize the message
    DeserializationError error = deserializeJson(doc,message);
    if(error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      messageReady = false;
      return;
    }
    if(doc["type"] == "request") {
      doc["type"] = "response";
      // Get data from analog sensors
      doc["latitude"] = latitude;
      doc["longitude"] = longitude;
      serializeJson(doc,Serial);
    }
    messageReady = false;
  }
}

//static void smartDelay(unsigned long ms) 
//  {
//  unsigned long start = millis();
//  do 
//    {
//      while (gpsserial.available())
//        gps.encode(gpsserial.read());
//    } 
//  while (millis() - start < ms);
//  }

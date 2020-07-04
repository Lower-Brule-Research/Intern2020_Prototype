/*------------------------------------------------------------------------------
This code get's loaded on to the Arduino!
------------------------------------------------------------------------------*/
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

String message = "";
bool messageReady = false;
String gpslat;
String gpslong;

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
}

void loop() {
    smartDelay(1000);
    gpslat = String(gps.location.lat(), 6);
    gpslong = String(gps.location.lng(), 6);
    response();
  }
 
void response() {
      DynamicJsonDocument doc(1024);
      doc["type"] = "response";
      doc["latitude"] = gpslat;
      doc["longitude"] = gpslong;
      serializeJson(doc,Serial);
  }

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

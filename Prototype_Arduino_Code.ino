/*------------------------------------------------------------------------------
This code get's loaded on to the Arduino!
------------------------------------------------------------------------------*/

//Libraries needed
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <virtuabotixRTC.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);   //Wiring Diagram https://surtrtech.com/2018/01/27/how-to-simply-use-ds1302-rtc-module-with-arduino-board-and-lcd-screen/

virtuabotixRTC myRTC(6, 7, 8);  //If you change the wiring change the pins here also

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
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  myRTC.setDS1302Time(15, 31, 15, 5, 12, 1, 2018); //Here you write your actual time/date as shown above 
  //but remember to "comment/remove" this function once you're done and the module will continue counting it automatically
  ss.begin(GPSBaud);
}

void loop() {
  
   //Here code displays time and date on the LCD
   lcd.clear(); 
   myRTC.updateTime();
   lcd.setCursor(0,0);
   lcd.print(myRTC.dayofmonth);
   lcd.print("/");
   lcd.print(myRTC.month);
   lcd.print("/");
   lcd.print(myRTC.year);
   lcd.setCursor(0,1);
   lcd.print(myRTC.hours);
   lcd.print(":");
   lcd.print(myRTC.minutes);
   lcd.print(":");
   lcd.print(myRTC.seconds);
  
  
    //Here GPS Data gets sent to the ESP8266 via a JSON response
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

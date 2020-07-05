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
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"          //Heart rate calculating algorithm

MAX30105 particleSensor; // initialize MAX30102 with I2C

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

void setup()
{

}



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

  //   Serial.begin(115200); Originally this is what the heart rate monitor used. Not sure if we can use 9600 Baud.
  while(!Serial); //We must wait for Teensy to come online
  delay(100);
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("MAX30102");
  delay(100);
  
  // Initialize sensor
  if (particleSensor.begin(Wire, I2C_SPEED_FAST) == false) //Use default I2C port, 400kHz speed
  {
    lcd.print("MAX30105 not found");
    while (1);
  }

  byte ledBrightness = 70; //Options: 0=Off to 255=50mA
  byte sampleAverage = 1; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  int sampleRate = 400; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 69; //Options: 69, 118, 215, 411
  int adcRange = 16384; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
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
   delay(1000);
  
   //Here the Heart Rate Sensor gets checked and the AVG BPM gets displayed on the LCD.
   particleSensor.check(); 
   while (particleSensor.available()) {
      particleSensor.nextSample();  
      long irValue = particleSensor.getIR();    //Reading the IR value it will permit us to know if there's a finger on the sensor or not

      long delta = millis() - lastBeat;                   //Measure duration between two beats
      lastBeat = millis();
  
      beatsPerMinute = 60 / (delta / 1000.0);           //Calculating the BPM
      
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
      lcd.print(", Avg BPM=");
      lcd.print(beatAvg);
      }
     }
  
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

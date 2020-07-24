/*------------------------------------------------------------------------------
This code get's loaded on to the Arduino and will display the time or heartrate 
depending on which button is pressed
------------------------------------------------------------------------------*/
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"          //Heart rate calculating algorithm
#include <virtuabotixRTC.h> //Libraries needed
#include <LiquidCrystal_I2C.h>

MAX30105 particleSensor; // initialize MAX30102 with I2C
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

LiquidCrystal_I2C lcd(0x27,16,2); //Wiring CLK -> 6 , DAT -> 7, Reset -> 8
virtuabotixRTC myRTC(6, 7, 8); //If you change the wiring change the pins here also

int timeButton= 9;
int heartButton= 10;

void setup() {
  Serial.begin(9600);
  pinMode(timeButton, INPUT);
  pinMode(heartButton, INPUT);
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  myRTC.setDS1302Time(15, 31, 15, 5, 12, 1, 2018); //Here you write your actual time/date 
  //but remember to "comment/remove" this function once you're done as I did
  //The setup is done only one time and the module will continue counting it automatically
  
  // Initialize sensor
    if (particleSensor.begin(Wire, I2C_SPEED_FAST) == false) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
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
  int heartState = digitalRead(heartButton); //read the input pin
  if (heartState == 1){   //if the button is pressed
    particleSensor.check(); //Check the sensor
    while (particleSensor.available()) {
      particleSensor.nextSample();  
      long irValue = particleSensor.getIR();    //Reading the IR value it will permit us to know if there's a finger on the sensor or not
      if (checkForBeat(irValue) == true)                        //If a heart beat is detected
      {
        long delta = millis() - lastBeat;                   //Measure duration between two beats
        lastBeat = millis();
        beatsPerMinute = 60 / (delta / 1000.0);           //Calculating the BPM
        if (beatsPerMinute < 255 && beatsPerMinute > 20)               //To calculate the average we strore some values (4) then do some math to calculate the average
        {
          rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
          rateSpot %= RATE_SIZE; //Wrap variable
  
          //Take average of readings
          beatAvg = 0;
          for (byte x = 0 ; x < RATE_SIZE ; x++)
            beatAvg += rates[x];
          beatAvg /= RATE_SIZE;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("BPM=");
          lcd.print(beatAvg);
          delay(1000);
          Serial.println(beatAvg);
          int timeState = digitalRead(timeButton); //read the input pin
          if (timeState == 1){   //if the button is pressed
            break;
          }
        }
      }
    }
  }
  lcd.clear(); //Here after clearing the LCD we take the time from the module and print it on the screen with usual LCD functions
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
}

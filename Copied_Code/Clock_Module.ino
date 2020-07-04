//This code is to use with DS1302 RTC module, it permits you to setup the actual time and date
//And you can visualize them on the LCD i2c screen
//This code is a modified version of the code provided in virtuabotixRTC library
//Refer to Surtrtech Youtube chhannel/Facebook page for more information

#include <Wire.h>
#include <virtuabotixRTC.h> //Libraries needed
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);
//Wiring SCLK -> 6, I/O -> 7, CE -> 8
//Or CLK -> 6 , DAT -> 7, Reset -> 8

virtuabotixRTC myRTC(6, 7, 8); //If you change the wiring change the pins here also

void setup() {
 Serial.begin(9600);
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  myRTC.setDS1302Time(15, 31, 15, 5, 12, 1, 2018); //Here you write your actual time/date as shown above 
  //but remember to "comment/remove" this function once you're done as I did
  //The setup is done only one time and the module will continue counting it automatically
 
}

void loop() {
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

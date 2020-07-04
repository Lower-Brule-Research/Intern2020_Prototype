/*------------------------------------------------------------------------------
This gets loaded onto the ESP8266
------------------------------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server;
char* ssid = "Your Network Name";
char* password = "Your Password";

void setup()
{
  WiFi.begin(ssid,password);
  Serial.begin(9600);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/",handleIndex);
  server.begin();
}

void loop()
{
  server.handleClient();
}

void handleIndex()
{
  DynamicJsonDocument doc(1024);
  double latitude = 0, longitude = 0;
  // Reading the response
  boolean messageReady = false;
  String message = "";
  while(messageReady == false) { // blocking but that's ok
    if(Serial.available()) {
      message = Serial.readString();
      messageReady = true;
    }
  }
  // Attempt to deserialize the JSON-formatted message
  DeserializationError error = deserializeJson(doc,message);
  if(error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  latitude = doc["latitude"];
  longitude = doc["longitude"];
  // Prepare the data for serving it over HTTP
  String output = "latitude: " + String(latitude) + "\n";
  output += "longitude: " + String(longitude);
  // Serve the data as plain text, for example
  server.send(200,"text/plain",output);
}

// https://www.youtube.com/watch?v=4e9hE34RMZM

#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <TinyGPS++.h>

const char* ssid = "LBDS"; // Enter the SSID of your WiFi Network.
const char* password = "#BRul3!!";// Enter the Password of your WiFi Network.
const int gp0_Pin = 2;
int gp0_State = 0;

const char *host = "api.pushingbox.com";                  // pushingbox Host

WiFiClient espClient;
TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.println("");
  Serial.println("");
  Serial.print("Connecting To: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(gp0_Pin, INPUT_PULLUP);
 }

void loop() {
  

  gp0_State = digitalRead(gp0_Pin);
  if (gp0_State==0) {
      
      smartDelay(1000);
      String Button_status = String(gps.location.lat(), 6);
      String LdrValueSend = String(gps.location.lng(), 6);                   
      Serial.println("");
      Serial.println("----------------------------------------------");
      Serial.print("connecting to ");
      Serial.println(host);
    
      // Use WiFiClient class to create TCP connections
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
    
      // We now create a URI for the request
      String url = "/pushingbox?";
      url += "devid=";
      url += "vDBD436D217B61B2";
      url += "&LDR="+LdrValueSend;
      url += "&Button="+Button_status;
    
      Serial.print("Requesting URL: ");
      Serial.println(url);
    
      // This will send the request to the server
      client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 4000) {
          Serial.println(">>> Client Timeout !");
          client.stop();
          return;
        }
      }
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
      Serial.print("Data Sent!");
    }
    
    Serial.println();
    Serial.println("closing connection");
    Serial.println("----------------------------------------------");
    delay(1000);
  }
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial.available())
      gps.encode(Serial.read());
  } while (millis() - start < ms);
}

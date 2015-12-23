#include <ESP8266WiFi.h>
//#include <DNSServer.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <Wire.h>
#include "mq135-data.h"
#include "wifi-utils.h"
#include "wifi-creds.h"


int status = WL_IDLE_STATUS;     // the Wifi radio's status


void setup() {
  delay(1000);
  Serial.begin(115200);
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true)
    delay(10000);
  }

  
 // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:    
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
   
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
}



void loop() {
  //dns.processNextRequest();  
  //server.handleClient();
  
  wifiCheckReconnect(status,ssid,pass);
  
  uint16_t valr = analogRead(A0);
  uint16_t val =  ((float)22000*(1023-valr)/valr); 
  float mq135_ro = mq135_getro(15231, 660);//8000;//mq135_getro(val, 500);
  //convert to ppm (using default ro)
  float valAIQ = mq135_getppm(val, mq135_ro);
  
  delay(10000);
  printCurrentNet();
    Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect("co2.jehy.ru", 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /send.php?data={\"id\":1,\"val\":"+String(valr)+",\"ppm\":"+String(valAIQ)+"} HTTP/1.1");
    client.println("Host: co2.jehy.ru");
    client.println("Connection: close");
    client.println();
    client.stop();
  }
}



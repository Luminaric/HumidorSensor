/*
 * This sketch writes directly on the ESP chip.
 * The ESP8266WiFi.h library is used here.
 * This sensor is battery powered and uses the ESP12 
 * chip. The chip is put into sleep between transmissions.
 * The DHT22 is used to get the Temp and Humidity. Battery 
 * voltage is also sampled and transmitted to the MySQL server.
 * The Temp and Humidity are also sent to ThingSpeak.
 * 
 * Version:   1.2
 * SensorID:  HUMIDOR
 * Platform:  ESP8266
 * Date:      2016-01-22
 * Requires:
 * Hardware:  Uses the DHT22 Temp and Humid sensor.
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "ethernetSettings.h"
#include <DHT.h>

extern "C" {
#include "user_interface.h"
uint16 readvdd33(void);
}
#define THING_SPEAK           "POST /update HTTP/1.1\n\rHost: api.thingspeak.com\n\rConnection:close\n\rX-THINGSPEAKAPIKEY: JABI4GT3AMXAQ8MW\n\rContent-type: application/x-www-form-urlencoded\n\rContent-Length: "
#define MYSQL_OPEN_STRING     "Get /add2.php?f0="//this is the URL for the PI server
#define CLOSE_STRING          "Host: 10.1.1.25 Connection: close"
#define RESET_PIN             16
#define DHTPIN                2
#define DHTTYPE               DHT22
#define SENSORID              "HUMIDOR"//change as required

uint16 x;
float t, h;
float v = readvdd33() / 1000.0;
String getString;
const unsigned long sleepTimeS = 3600;//in seconds 3600
const unsigned long multiplier = 1000000;//cycles for one second

DHT dht(DHTPIN, DHTTYPE, 16);
//
WiFiClient client;

void sendThingSpeak() {
  //make the IOT string
  String getIOTString = "field1=";
  getIOTString += SENSORID;
  getIOTString += "&field2=";
  getIOTString += t*1.8+32;
  getIOTString += "&field3=";
  getIOTString += h;
  getIOTString += "&field4=";
  getIOTString += v;
  Serial.print("iotString: ");
  Serial.println(getIOTString);
  //Send the data to ThingSpeak
  if (client.connect(serverTS, 80))  { 
      client.print(THING_SPEAK);
      client.print(getIOTString.length());
      client.print("\n\n");
      client.print(getIOTString);
      client.stop();
  
    }
  }
  void getData(void) {
    h = dht.readHumidity();
    t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT Sensor);
      /*some sort of function to log the 
       * error should exist here */
       ESP.restart();
    }
    Serial.print("Temp ");Serial.print(t);Serial.print("|");Serial.print("Humidity ");Serial.println(h);
  }
  void sendMySQL(void)  {
    getString = MYSQL_OPEN_STRING;
    getString += SENSORID;
    getString += "&f1=";
    getString += t;
    getString += "&f2=";
    getString += h;
    getString += "&f3=";
    getString += 0x00;
    getString += "&f4=";
    getString += v;
    Serial.println(getString);
    client.println(getString);
    client.println(CLOSE_STRING);
    client.println();
    client.println();
    client.stop();
  }
void  dhtTest() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    for(;;);
  }
}

void setup() {
  //Init DHT Sensor
  dht.begin();
  //Setup Serial and report status
  Serial.begin(9600);
  //dhtTest();
  Serial.print("DHT using PIN: ");
  Serial.println(DHTPIN);
  //Connect Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    x += 1;
    //need to add logic here in case the ESP fails to connect to
    //Wi-Fi
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Use WiFiClient class to create TCP connections
  if (!client.connect(serverLH, 80)) {
    Serial.println("connection failed");
   return;
  } else  {
    getData();
    sendMySQL();
    sendThingSpeak(); 
  }
  
  delay(1000);
  system_deep_sleep_set_option(0);
  system_deep_sleep((sleepTimeS * multiplier) - micros());


}

void loop() {
  delay(6000);

}



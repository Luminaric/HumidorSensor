/*
 * This sketch writes directly on the ESP chip.
 * The ESP8266WiFi.h library is used here.
 * This sensor is battery powered and uses the ESP12 
 * chip. The chip is put into sleep between transmissions.
 * The DHT22 is used to get the Temp and Humidity. Battery 
 * voltage is also sampled and transmitted to the MySQL server.
 * The Temp and Humidity are also sent to IO.ADAFRUIT.
 * 
 * Version:   1.5
 * SensorID:  HUMIDOR
 * Platform:  ESP8266
 * Date:      2016-01-27
 * Requires:
 * Hardware:  Uses the DHT22 Temp and Humid sensor. It also
 * uses the Adafruit IO REST API
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "Adafruit_IO_Client.h"
#include "ethernetSettings.h"
#include <DHT.h>

extern "C" {
#include "user_interface.h"
uint16 readvdd33(void);
}

#define MYSQL_OPEN_STRING     "Get /add2.php?f0="//this is the URL for the PI server
#define CLOSE_STRING          "Host: 10.1.1.25 Connection: close"
#define DHTPIN                2
#define DHTTYPE               DHT22
#define SENSORID              "HUMIDOR"//change as required

float t, h;
float v = readvdd33() / 1000.0;
String getString;
const unsigned long sleepTimeS = 3600;//in seconds
const unsigned long multiplier = 1000000;//cycles for one second

DHT dht(DHTPIN, DHTTYPE, 16);
//
WiFiClient client;

Adafruit_IO_Client aio = Adafruit_IO_Client(client, AIO_KEY);

Adafruit_IO_Feed humidity = aio.getFeed("humidor-humidity");
Adafruit_IO_Feed temperature = aio.getFeed("humidor-temp");

void sendThingSpeak() {
  char sH[7], sT[7];
  dtostrf(h,5,1,sH);
  dtostrf(t,5,1,sT);
  //send Humidity
  if(humidity.send(sH)) {
    Serial.print(F("Wrote humidity to feed: ")); Serial.println(sH);
  } else  {
    Serial.println(F("Error writing value to feed!"));
  }
  //send Temperature
  if(temperature.send(sT)) {
    Serial.print(F("Wrote termperature to feed: ")); Serial.println(sT);
  } else  {
    Serial.println(F("Error writing value to feed!"));
  }
  }
  void getData(void) {
    h = dht.readHumidity();
    t = dht.readTemperature();
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
 // system_deep_sleep_set_option(0);
 // system_deep_sleep((sleepTimeS * multiplier) - micros());
ESP.deepSleep(60000000, WAKE_RF_DEFAULT);

}

void loop() {
  delay(6000);

}



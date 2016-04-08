/*
  This sketch writes directly on the ESP chip.
  The ESP8266WiFi.h library is used here.
  This sensor is battery powered and uses the ESP12 
  chip. The chip is put into sleep between transmissions.
  The DHT22 is used to get the Temp and Humidity. Battery 
  voltage is also sampled and transmitted to the MQTT server.
  
  Version:   2.0
  SensorID:  HUMIDOR
  Platform:  ESP8266
  Date:      2016-03-17
  Requires:
  Hardware:  Uses the DHT22 Temp and Humid sensor. 

  Update history
  2016-03-17
  Totally rewritten to use MQTT.
  2016-03-30
  Modified the DHT.cpp file.  Increased the delay after
  setting pinMode to 40 from 10 -- didn't work
  2016-04-01
  Switched Libraries(not saved here) from the standard
  DHT library to the PietteTech DHT library since some
  posters reported success with it and this problem. 
  -- Didn't fix the problem
  2016-04-07
  Yet another poster reported that setting the DHTPIN
  as an OUTPUT and setting it state to LOW before calling
  the sleep function work.
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "ethernetSettings.h"
#include <DHT.h>
#include <PubSubClient.h>

extern "C" {
#include "user_interface.h"
uint16 readvdd33(void);
}
#define DHTPIN                2
#define DHTTYPE               DHT22
#define SENSORID              "HUMIDOR"//change as required

DHT dht(DHTPIN, DHTTYPE, 15);
WiFiClient wireless;
PubSubClient client(wireless);

const char eStr[50] = " returned a invalid result";
const unsigned long sleepTimeS = 3600;//60min = 3600, 30min = 1800
const unsigned long multiplier = 1000000;//cycles for one second
char voltage[7],humidity[7],temperatureC[7];
float h,t;
bool dataFlag;

void getData () {
  h = dht.readHumidity();
  t = dht.readTemperature(); 
  if (!isnan(t) || !isnan(h) || !h == 0 || !t == 0) {   
    Serial.print(h);Serial.print(" | ");Serial.println(t);
    dtostrf(h,4,1,humidity);
    dtostrf(t,4,1,temperatureC);
    dtostrf(readvdd33()/1000.0,4,2,voltage);
    dataFlag = true;
    return;
  }
  dataFlag = false;
  //String eString ="Sensor had problems";
  //eString.toCharArray(error, 100);
  //humidity="0.00";temperature="0.00";
}
void setup() {
  /*--------------------------------------*/
  /*      Send Sensor Info to Serial      */
  /*--------------------------------------*/
  Serial.begin(9600);
  Serial.print(SENSORID);
  Serial.print(" is using Pin ");
  Serial.print(DHTPIN);
  Serial.println(" For DHT sensor.");
  delay(2000);//added this 3/24 to see if a delay gives the sensor
              //a chance to get a good reading after recovering
              //from uP deep sleep.
  dht.begin();
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(server,1883);
  /*--------------------------------------*/
  /*                 Get Data             */
  /*--------------------------------------*/
  getData();
  /*--------------------------------------*/
  /*           Put DHTPIN to LOW          */
  /* Testing to see if DHT return values  */
  /* after deepsleep. Doing this with PIN2*/
  /* causes the ESP to lock up.           */  
  /*--------------------------------------*/
  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, LOW);   
  /*--------------------------------------*/
  /*            Connect to MQTT           */
  /*--------------------------------------*/  
  if ( dataFlag == true)  {
  if(!client.connected()) {
    client.connect(SENSORID);
    Serial.println("Connected");
    Serial.print("rc=");
    Serial.println(client.state());
  }
  /*--------------------------------------*/
  /*             Send to MQTT             */
  /*--------------------------------------*/
  String mqttString = "[\"";
  mqttString += SENSORID;
  mqttString += "\",{\"temperature\":\"";
  mqttString += temperatureC;
  mqttString += "\",\"humidity\":\"";
  mqttString += humidity;
  mqttString += "\",\"voltage\":\"";
  mqttString += voltage;
  mqttString += "\"}]";
  Serial.print("MQTT String: ");
  Serial.println(mqttString);
  char payload[100];
  mqttString.toCharArray(payload, 100);
  client.publish("Sensors",payload);
  char pload[50];
  String logString = "Sensor ";
  logString += SENSORID;
  logString += " Trasmission Successful";
  logString.toCharArray(pload, 50);
  client.publish("Log",pload);
  } else if (dataFlag == false) {
      if(!client.connected()) {
        client.connect(SENSORID);
        Serial.println("Connected");
        Serial.print("rc=");
        Serial.println(client.state());
      }
      char pload[50];
      String logString = "Sensor ";
      logString += SENSORID;
      logString += eStr;
      logString.toCharArray(pload, 50);
      client.publish("Log", pload);
  }
  delay(1000);
//  system_deep_sleep_set_option(0);
//  system_deep_sleep((sleepTimeS * multiplier) - micros());
  ESP.deepSleep(sleepTimeS * multiplier);
}

void loop() {
  delay (10000);
l
}
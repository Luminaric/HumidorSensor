# HumidorSensor
Battery powered Wi-Fi sensor based on the ESP8266 ESP-12.

This sketch uses the Arduino IDE to program the ESP.  The DHT Master Library is used to get data from a DHT-22 sensor. The ESP-12 has RST wired to the GPIO16 pin so it can be started after sleeping for a predetermined amount of time. The entire code is done in the setup portion of the program since this sketch is run only once before the ESP is put to deep sleep to conserve battery.

Pin Connections:
RST --> GPIO16
CP-PD --> Vcc via 4k7ohm resister
DHT-22 --> GPIO2

The sensor results are sent to a Mosquitto MQTT broker. From there a Python script running on the server sends the data to my home MySQL server for storage and also sent to ThingSpeak so I can view the reading on my smart phone.

The original version of this program running on the perfboard version of the circuit worked pefrectly for weeks. Moving to a hard wired board and uploading the orignal version of the program doesn't work. When the ESP8266 returns from deepsleep, the DHT22 no longer sends data.

Updates:

I first tried to switch DHT libraries. From the ESP8266.com forum posters were saying they had success with the PietteTech DHT library. The version of code that I used for that experiment I did not publish since it didn't work. 

In another post I see where another poster has had success with putting the DHTPIN as an output pin and setting it to LOW.


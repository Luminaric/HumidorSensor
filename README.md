# HumidorSensor
Battery powered Wi-Fi sensor based on the ESP8266 ESP-12.

This sketch uses the Arduino IDE to program the ESP.  The DHT Master Library is used to get data from a DHT-22 sensor. The ESP-12 has RST wired to the GPIO16 pin so it can be started after sleeping for a predetermined amount of time. The entire code is done in the setup portion of the program since this sketch is run only once before the ESP is put to deep sleep to conserve battery.

Pin Connections:
RST --> GPIO16
CP-PD --> Vcc via 4k7ohm resister
DHT-22 --> GPIO2

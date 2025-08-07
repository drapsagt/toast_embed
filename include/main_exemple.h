#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_AHTX0.h>

// WiFi credentials
const char *ssid = "WIFI_NETWORK_NAME"; // Replace with your WiFi SSID
const char *password = "WIFI_PASSWORD"; // Replace with your WiFi password

// MQTT Broker settings
const char *mqtt_broker = "MQTT_BROKER_IP"; // Replace with your MQTT broker IP
const int mqtt_port = 1883;
const char *mqtt_username = "MQTT_USERNAME"; // Leave empty if no auth
const char *mqtt_password = "MQTT_PASSWORD"; // Leave empty if no auth
const char *client_id = "ESP32_Client";

#endif // MAIN_H
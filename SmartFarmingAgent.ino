/***************************************************
 * File: SmartFarmingAgent.ino
 * Each zone controls irrigation using local ML or thresholds.
 ****************************************************/
#include <WiFi.h>
#include <PubSubClient.h>
#include "SoilMoistureSensor.h"
#include "TinyMLModel.h" // hypothetical library or your TFLite code

// Wi-Fi & broker
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASS";
const char* mqttServer = "192.168.1.150";
const int   mqttPort   = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// pins
#define VALVE_PIN  14
float moistureThreshold = 35.0; // fallback threshold

void setup() {
  Serial.begin(115200);
  pinMode(VALVE_PIN, OUTPUT);
  digitalWrite(VALVE_PIN, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  client.setServer(mqttServer, mqttPort);
  connectMqtt();

  // init sensor, tinyml model
  initSoilSensor();
  initTinyMLModel();
}

void loop() {
  if (!client.connected()) {
    connectMqtt();
  }
  client.loop();

  float currentMoist = readSoilMoisture();
  float predictedMoist = runTinyMLInference(currentMoist);
  // if predicted dryness < 30, open valve
  if (predictedMoist < 30) {
    digitalWrite(VALVE_PIN, HIGH);
  } else {
    digitalWrite(VALVE_PIN, LOW);
  }
  publishData(currentMoist, predictedMoist);
  delay(60000); // check every 1 min
}

void connectMqtt() {
  while (!client.connected()) {
    if (client.connect("ZoneAgentA")) {
      // subscribed topics if needed
    } else {
      delay(1000);
    }
  }
}

void publishData(float cm, float pm) {
  // publish JSON with "current" and "predicted"
  String payload = "{\"current\":";
  payload += cm;
  payload += ",\"predicted\":";
  payload += pm;
  payload += "}";
  client.publish("zoneA/soilStatus", payload.c_str());
}

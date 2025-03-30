/*********************************************************
 * File: RetailShelfAgent.ino
 * Each shelf has a load cell + HX711. If the weight
 * indicates low stock, it requests reorder from
 * store inventory system.
 *********************************************************/
#include <WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"

const char* ssid = "RetailStoreWiFi";
const char* password = "Store1234";
const char* mqttServer = "192.168.10.10";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
HX711 scale;

#define DOUT_PIN 4
#define SCK_PIN  5

String shelfID = "A1";
float thresholdWeight = 500.0; // grams

void setup() {
  Serial.begin(115200);
  scale.begin(DOUT_PIN, SCK_PIN);
  scale.set_scale(2280.f); // calibration factor
  scale.tare();

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) { delay(500); }
  client.setServer(mqttServer, mqttPort);
  connectMqtt();
}

void loop() {
  if(!client.connected()) connectMqtt();
  client.loop();

  float weight = scale.get_units(5);
  if(weight < thresholdWeight) {
    // publish reorder request
    String pay = "{\"shelf\":\"" + shelfID + "\",\"item\":\"SKU123\",\"weight\":" + String(weight) + "}";
    client.publish("store/inventory", pay.c_str());
  }
  delay(10000);
}

void connectMqtt() {
  while(!client.connected()) {
    if(client.connect("ShelfAgentA1")) {
      // subscribe if needed
    } else {
      delay(1000);
    }
  }
}

/*****************************************************
 * File: EnergyAgent.ino
 * Each house node has solar, battery, and usage data
 * Publishes offers or bids on a shared broker
 *****************************************************/
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Wi-Fi + MQTT
const char* ssid = "HomeGridWiFi";
const char* password = "GridPass123";
const char* mqttServer = "192.168.1.220";
const int   mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

float predictedGen = 0.0;
float predictedLoad = 0.0;
float batteryLevel = 50.0; // in %

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  client.setServer(mqttServer, mqttPort);
  connectMqtt();

  // init power measurement
  // init local ML model for generation forecast
}

void loop() {
  if (!client.connected()) {
    connectMqtt();
  }
  client.loop();

  // measure current usage
  float currUsage = readCurrentUsage();
  batteryLevel = readBatteryLevel();

  // run forecast model
  predictedGen = runGenForecast();
  predictedLoad = runLoadForecast();

  // if predictedGen > predictedLoad, publish an "offer"
  // otherwise publish a "bid"
  float net = predictedGen - predictedLoad;
  if (net > 0) {
    publishOffer(net);
  } else {
    publishBid(-net);
  }

  delay(60000);
}

void connectMqtt() {
  while (!client.connected()) {
    if (client.connect("HouseAgentA")) {
      // subscribed topics if needed
    } else {
      delay(1000);
    }
  }
}

void publishOffer(float surplus) {
  DynamicJsonDocument doc(128);
  doc["type"] = "offer";
  doc["amount"] = surplus;
  doc["battery"] = batteryLevel;
  String payload;
  serializeJson(doc, payload);
  client.publish("microgrid/offers", payload.c_str());
}

void publishBid(float deficit) {
  DynamicJsonDocument doc(128);
  doc["type"] = "bid";
  doc["amount"] = deficit;
  doc["battery"] = batteryLevel;
  String payload;
  serializeJson(doc, payload);
  client.publish("microgrid/bids", payload.c_str());
}

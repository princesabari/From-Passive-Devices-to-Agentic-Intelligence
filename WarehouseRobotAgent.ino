/**********************************************************
 * File: WarehouseRobotAgent.ino
 * Demonstration code for an agentic picking robot
 * using MQTT for task assignment and status updates.
 **********************************************************/
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT settings
const char* mqtt_server = "192.168.1.100";
const int   mqtt_port   = 1883;
const char* clientID    = "RobotAgent1";
const char* taskTopic   = "tasks/queue";
const char* statusTopic = "robot1/status";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, taskTopic) == 0) {
    // parse JSON payload for item location, etc.
    DynamicJsonDocument doc(200);
    deserializeJson(doc, payload, length);
    const char* itemID = doc["item"];
    const char* location = doc["loc"];
    // handle picking logic
    pickItem(itemID, location);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  connectToMqtt();

  // init motors, sensors
}

void loop() {
  if (!client.connected()) {
    connectToMqtt();
  }
  client.loop();

  // read sensor data, update status
  String statusPayload = buildStatusPayload();
  client.publish(statusTopic, statusPayload.c_str());

  delay(3000);
}

void connectToMqtt() {
  while (!client.connected()) {
    if (client.connect(clientID)) {
      client.subscribe(taskTopic);
    } else {
      delay(1000);
    }
  }
}

String buildStatusPayload() {
  // example JSON: {"state":"idle","battery":84}
  StaticJsonDocument<100> doc;
  doc["state"] = "idle";
  doc["battery"] = 84; // placeholder
  String output;
  serializeJson(doc, output);
  return output;
}

void pickItem(const char* itemID, const char* location) {
  // robot navigation logic here
  // publish updates to statusTopic if needed
}

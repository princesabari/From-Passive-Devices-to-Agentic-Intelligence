/*************************************************************
 * File: DoorSecurityAgent.ino
 * Description: Runs on ESP32 controlling a door strike or relay,
 * and checks badge or basic face rec model. If uncertain, queries
 * a central service via MQTT or HTTP.
 *************************************************************/
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPIFFS.h>
// Include code for RFID or camera if needed...

const char* ssid = "OfficeWiFi";
const char* password = "SuperSecret123";
const char* mqtt_server = "192.168.1.210";
const int mqtt_port = 1883;
const char* clientID = "DoorAgentA";

#define RELAY_PIN 25

WiFiClient espClient;
PubSubClient client(espClient);

// A small local whitelist for demonstration
struct WhitelistEntry {
  String badgeID;
  bool   active;
};
WhitelistEntry localWhitelist[3] = {
  {"12345", true},
  {"99999", true},
  {"11111", false} // e.g., not active
};

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }

  client.setServer(mqtt_server, mqtt_port);
  connectMqtt();

  // init badge or camera code (pseudocode)
  // initBadgeReader();
  // or initFaceModel();
}

void loop() {
  if (!client.connected()) connectMqtt();
  client.loop();

  // Suppose we read a badge from the RFID
  String scannedID = readBadge(); // pseudocode
  if (scannedID.length() > 0) {
    bool access = checkLocalList(scannedID);
    if (!access) {
      access = remoteCheck(scannedID);
    }
    if (access) {
      openDoor();
    } else {
      logEvent(scannedID, false);
    }
  }
  delay(200);
}

void connectMqtt() {
  while (!client.connected()) {
    if (client.connect(clientID)) {
      // subscribe if needed
    } else {
      delay(1000);
    }
  }
}

bool checkLocalList(String badge) {
  for (int i=0; i<3; i++) {
    if (localWhitelist[i].badgeID == badge && localWhitelist[i].active) {
      logEvent(badge, true);
      return true;
    }
  }
  return false;
}

bool remoteCheck(String badge) {
  // Possibly publish or do a synchronous HTTP request
  // Pseudocode: check with central DB
  // if verified => return true
  return false;
}

void openDoor() {
  digitalWrite(RELAY_PIN, HIGH);
  delay(3000); // door open 3s
  digitalWrite(RELAY_PIN, LOW);
}

void logEvent(String badge, bool allowed) {
  // e.g., publish to "security/logs"
  String payload = "{\"badge\":\"" + badge + "\",\"access\":" + (allowed?"true":"false") + "}";
  client.publish("security/logs", payload.c_str());
}

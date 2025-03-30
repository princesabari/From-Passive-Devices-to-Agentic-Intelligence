/******************************************************
 * File: MaintenanceAgent.ino
 * Reads vibration from a MEMS accelerometer, runs local
 * anomaly detection or forecast, publishes "predicted
 * downtime" messages.
 ******************************************************/
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "FactoryNet";
const char* password = "Mach1n3!";
const char* broker = "192.168.1.40";

WiFiClient espClient;
PubSubClient client(espClient);

// Suppose we have an accelerometer via I2C
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) { delay(500); }

  client.setServer(broker, 1883);
  connectMqtt();

  initAccelerometer();
  initVibrationModel(); // TFLite or classical
}

void loop() {
  if(!client.connected()) connectMqtt();
  client.loop();

  float vibData[128];
  readVibrationWindow(vibData, 128);

  float anomalyScore = runVibrationInference(vibData, 128);
  if(anomalyScore > 0.7) {
    String pay = "{\"machine\":\"CNC1\",\"anomalyScore\":";
    pay += anomalyScore;
    pay += ",\"predict\":\"48h\"}";
    client.publish("factory/predictive", pay.c_str());
  }
  delay(5000);
}

void connectMqtt() {
  while(!client.connected()) {
    if(client.connect("CNC1_Agent")) {
      // subscribed topics if needed
    } else {
      delay(1000);
    }
  }
}

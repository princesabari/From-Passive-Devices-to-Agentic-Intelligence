/********************************************************************************
 * File: MQTT_SmartHub.ino
 * Board: ESP32 (e.g., DevKit V1)
 *
 * Description:
 *  1. Connects to Wi-Fi
 *  2. Publishes sensor data (Temp, Hum, Motion, Light) via MQTT
 *  3. Subscribes to a command topic to toggle a relay/LED
 *  4. Uses a simple local rule-based logic as well
 *
 * Author: Your Name
 * LinkedIn: https://www.linkedin.com/in/sabarinathms/
 ********************************************************************************/

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ----------- Wi-Fi & MQTT Credentials -----------
const char* ssid       = "YourWiFi";
const char* password   = "YourPassword";
const char* mqttServer = "test.mosquitto.org";  // Example public broker
const int   mqttPort   = 1883;

// You can set unique MQTT topics for your device
#define PUBLISH_TOPIC_TEMPHUM "esp32/sensors/temphum"
#define PUBLISH_TOPIC_MOTION  "esp32/sensors/motion"
#define PUBLISH_TOPIC_LIGHT   "esp32/sensors/light"
#define SUBSCRIBE_TOPIC_CMD   "esp32/cmd"

// ----------- MQTT Client & Buffers -----------
WiFiClient espClient;
PubSubClient client(espClient);

// ----------- Sensor Pins & Config -----------
#define DHT_PIN  4
#define DHTTYPE  DHT22
#define PIR_PIN  16
#define LDR_PIN  34 // ADC
#define RELAY_PIN 26

DHT dht(DHT_PIN, DHTTYPE);

// Optional thresholds for local logic
float TEMP_THRESHOLD   = 28.0;
float HUM_THRESHOLD    = 70.0;
int   LIGHT_THRESHOLD  = 1000;

// ------------- Callback for subscribed topics -------------
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(" => ");

  String receivedMsg;
  for (int i = 0; i < length; i++) {
    receivedMsg += (char)message[i];
  }
  Serial.println(receivedMsg);

  // If this is the command topic, parse the message
  if (String(topic) == SUBSCRIBE_TOPIC_CMD) {
    // Example: "relay on" or "relay off"
    if (receivedMsg == "relay on") {
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Relay turned ON via MQTT command");
    } else if (receivedMsg == "relay off") {
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Relay turned OFF via MQTT command");
    }
  }
}

// ------------- Connect to Wi-Fi -------------
void setupWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 30) {
    delay(500);
    Serial.print(".");
    retries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed.");
  }
}

// ------------- Reconnect to MQTT if lost -------------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_Client")) {
      Serial.println("connected");
      // Subscribe to any needed topic
      client.subscribe(SUBSCRIBE_TOPIC_CMD);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in 5 seconds...");
      delay(5000);
    }
  }
}

// ------------- Local Rule-Based Logic -------------
void localAutomationLogic(float t, float h, int ldrVal) {
  // Example: If temp > TEMP_THRESHOLD or humidity > HUM_THRESHOLD => turn relay on
  if (t > TEMP_THRESHOLD || h > HUM_THRESHOLD) {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("[LocalLogic] Relay ON (temp/hum threshold)");
  } else if (ldrVal < LIGHT_THRESHOLD) {
    // If it's dark, also turn on the relay (maybe for a light)
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("[LocalLogic] Relay ON (dark environment)");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("[LocalLogic] Relay OFF");
  }
}

// ------------- Setup -------------
void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  dht.begin();
  setupWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  Serial.println("MQTT Smart Hub Setup Complete!");
}

// ------------- Main Loop -------------
void loop() {
  // Ensure MQTT connection
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read Sensors
  float temperature = dht.readTemperature();
  float humidity    = dht.readHumidity();
  int   motion      = digitalRead(PIR_PIN);
  int   ldrVal      = analogRead(LDR_PIN);

  // Quick DHT check
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Print for debugging
  Serial.printf("Temp: %.2fC | Hum: %.2f%% | Motion: %d | LDR: %d\n",
                temperature, humidity, motion, ldrVal);

  // Local rule-based control
  localAutomationLogic(temperature, humidity, ldrVal);

  // Publish sensor data over MQTT
  // 1. Temperature & Humidity
  String tempHumPayload = String("{\"temp\":") + temperature +
                          String(",\"hum\":") + humidity + "}";
  client.publish(PUBLISH_TOPIC_TEMPHUM, tempHumPayload.c_str());

  // 2. Motion
  client.publish(PUBLISH_TOPIC_MOTION, motion ? "1" : "0");

  // 3. Light
  client.publish(PUBLISH_TOPIC_LIGHT, String(ldrVal).c_str());

  delay(5000); // Adjust your sensor reading interval
}

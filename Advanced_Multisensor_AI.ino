/**************************************************************************************
 * File Name: Advanced_Multisensor_AI.ino
 * Target Board: ESP32 (DevKit V1 or similar)
 *
 * Features:
 *  1. Sensors:
 *      - DHT22 (temperature/humidity)
 *      - PIR (motion)
 *      - LDR (light level)
 *      - (Optional) MQ-2 (gas sensor)
 *  2. Local Web Server:
 *      - Serves a page showing current sensor readings and last 10 logs
 *  3. Rule-Based Automation:
 *      - If temperature/humidity cross a threshold, turn on a fan/relay
 *      - If motion is detected in low light, turn on a light/LED
 *  4. Optional Edge AI:
 *      - Placeholder for a TFLite Micro anomaly detection model
 *      - Demonstrates how you'd integrate local AI inference
 *  5. (Optional) Cloud Logging:
 *      - If you define ENABLE_CLOUD_LOGGING, it sends data to a mock endpoint
 *
 * Inline comments guide you through advanced features step by step.
 *
 * Author: Your Name
 * LinkedIn: https://www.linkedin.com/in/sabarinathms/
 **************************************************************************************/

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>

// Uncomment to enable placeholder cloud logging
// #define ENABLE_CLOUD_LOGGING

// Uncomment to enable placeholder TFLite Micro inference
// #define ENABLE_TFLITE_INFERENCE

#ifdef ENABLE_TFLITE_INFERENCE
// Include TFLite Micro headers here (requires library setup):
// #include "tensorflow/lite/micro/all_ops_resolver.h"
// #include "tensorflow/lite/micro/micro_error_reporter.h"
// #include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
// #include "model_data.h"  // Example model data
#endif

/************* Wi-Fi Configuration *************/
const char* ssid = "YourSSID";
const char* password = "YourPassword";

/************* Sensor Pins & Config ************/
#define DHTPIN  4      // GPIO4
#define DHTTYPE DHT22  // or DHT11
#define PIR_PIN  16    // Example pin for PIR
#define LDR_PIN  34    // ADC pin for LDR
#define FAN_PIN  26    // Relay or fan output
#define LIGHT_PIN 27   // LED or light output
// (Optional) Gas sensor pin if using MQ-2
#define GAS_PIN  35    // ADC pin for MQ-2

DHT dht(DHTPIN, DHTTYPE);

/************* Thresholds for Automation *************/
float TEMP_THRESHOLD    = 28.0;  // Turn on fan if above 28C
float HUM_THRESHOLD     = 80.0;  // Turn on fan if humidity above 80%
int   LIGHT_THRESHOLD   = 1000;  // If LDR reading < 1000 => low light
float GAS_THRESHOLD     = 400.0; // Example for MQ-2 sensor reading

/************* WebServer Setup *************/
WebServer server(80);

/************* Data Logging *************/
// We'll store the last 10 sensor readings in a struct ring buffer
#define LOG_SIZE 10

struct SensorLog {
  float temp;
  float hum;
  float gas; 
  bool  motion;
  int   lightVal;
  unsigned long timestamp;
};

SensorLog logs[LOG_SIZE];
int logIndex = 0;

/************* Function Prototypes *************/
void handleRoot();
void handleNotFound();
void updateSensorLog(float t, float h, float g, bool m, int l);
String generateHTMLPage();
void automationLogic(float t, float h, float g, bool m, int l);
void connectToWiFi();

#ifdef ENABLE_TFLITE_INFERENCE
void runTfLiteInference(float t, float h, float g, bool m, int l);
#endif

#ifdef ENABLE_CLOUD_LOGGING
void sendToCloudEndpoint(float t, float h, float g, bool m, int l);
#endif

/**************************************************************************************
 * SETUP
 **************************************************************************************/
void setup() {
  Serial.begin(115200);
  
  // Initialize DHT sensor
  dht.begin();

  // Setup pin modes
  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  // Additional pin for MQ-2 or other analog sensor
  pinMode(GAS_PIN, INPUT);

  digitalWrite(FAN_PIN, LOW);
  digitalWrite(LIGHT_PIN, LOW);

  // Connect to Wi-Fi
  connectToWiFi();

  // Start MDNS (optional: for local hostname access)
  if (MDNS.begin("esp32-advanced")) {
    Serial.println("MDNS responder started -> http://esp32-advanced.local/");
  }

  // Configure web server routes
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started on port 80");
  
  Serial.println("Advanced Multisensor AI Setup Complete!");
}

/**************************************************************************************
 * LOOP
 **************************************************************************************/
void loop() {
  // Handle incoming HTTP requests
  server.handleClient();

  // Read sensors
  float temperature = dht.readTemperature();
  float humidity    = dht.readHumidity();
  bool motion       = digitalRead(PIR_PIN);
  int  lightVal     = analogRead(LDR_PIN);
  float gasVal      = analogRead(GAS_PIN); // e.g., MQ-2

  // Quick error check for DHT
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Convert raw gas sensor reading if needed (placeholder)
  // e.g., float gasPPM = convertToPPM(gasVal);

  // Print to Serial for debugging
  Serial.printf("Temp: %.2f C, Hum: %.2f %%, Gas: %.2f, Motion: %d, Light: %d\n", 
                temperature, humidity, gasVal, motion, lightVal);

#ifdef ENABLE_TFLITE_INFERENCE
  // Run local AI inference if defined
  runTfLiteInference(temperature, humidity, gasVal, motion, lightVal);
#endif

  // Simple rule-based automation
  automationLogic(temperature, humidity, gasVal, motion, lightVal);

  // Update ring buffer logs
  updateSensorLog(temperature, humidity, gasVal, motion, lightVal);

#ifdef ENABLE_CLOUD_LOGGING
  // Optionally send data to a mock cloud endpoint
  sendToCloudEndpoint(temperature, humidity, gasVal, motion, lightVal);
#endif

  // Delay (adjust as needed; consider non-blocking approaches in production)
  delay(5000);
}

/**************************************************************************************
 * CONNECT TO WIFI
 **************************************************************************************/
void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi.");
  }
}

/**************************************************************************************
 * HANDLE ROOT - WEB SERVER
 **************************************************************************************/
void handleRoot() {
  // Generate dynamic HTML page with sensor logs
  String page = generateHTMLPage();
  server.send(200, "text/html", page);
}

/**************************************************************************************
 * HANDLE NOT FOUND
 **************************************************************************************/
void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}

/**************************************************************************************
 * GENERATE HTML PAGE
 **************************************************************************************/
String generateHTMLPage() {
  // Basic HTML to display the latest data and logs
  String html = "<!DOCTYPE html><html><head><title>ESP32 Advanced Monitor</title>";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<style> body { font-family: Arial; margin: 20px; } table { border-collapse: collapse; margin-top: 10px; } th, td { border: 1px solid #ccc; padding: 8px; } </style>";
  html += "</head><body>";

  // Latest reading
  SensorLog latest = logs[(logIndex - 1 + LOG_SIZE) % LOG_SIZE];
  html += "<h1>ESP32 Advanced Monitor</h1>";
  html += "<p><strong>Latest Reading:</strong></p>";
  html += "<ul>";
  html += "<li>Temperature: " + String(latest.temp, 2) + " &deg;C</li>";
  html += "<li>Humidity: " + String(latest.hum, 2) + " %</li>";
  html += "<li>Gas: " + String(latest.gas, 2) + "</li>";
  html += "<li>Motion: " + String(latest.motion ? "YES" : "NO") + "</li>";
  html += "<li>Light: " + String(latest.lightVal) + "</li>";
  html += "</ul>";

  // Log table
  html += "<h2>Last 10 Logs</h2>";
  html += "<table><tr><th>Timestamp</th><th>Temp</th><th>Hum</th><th>Gas</th><th>Motion</th><th>Light</th></tr>";
  for (int i = 0; i < LOG_SIZE; i++) {
    int idx = (logIndex + i) % LOG_SIZE;
    if (logs[idx].timestamp > 0) {
      html += "<tr>";
      html += "<td>" + String(logs[idx].timestamp) + "</td>";
      html += "<td>" + String(logs[idx].temp, 2) + "</td>";
      html += "<td>" + String(logs[idx].hum, 2) + "</td>";
      html += "<td>" + String(logs[idx].gas, 2) + "</td>";
      html += "<td>" + String(logs[idx].motion ? "YES" : "NO") + "</td>";
      html += "<td>" + String(logs[idx].lightVal) + "</td>";
      html += "</tr>";
    }
  }
  html += "</table>";

  html += "<p>Refresh page to see updates. (ESP32 local server)</p>";
  html += "</body></html>";

  return html;
}

/**************************************************************************************
 * UPDATE SENSOR LOG
 **************************************************************************************/
void updateSensorLog(float t, float h, float g, bool m, int l) {
  logs[logIndex].temp      = t;
  logs[logIndex].hum       = h;
  logs[logIndex].gas       = g;
  logs[logIndex].motion    = m;
  logs[logIndex].lightVal  = l;
  logs[logIndex].timestamp = millis();
  logIndex = (logIndex + 1) % LOG_SIZE;
}

/**************************************************************************************
 * AUTOMATION LOGIC
 **************************************************************************************/
void automationLogic(float t, float h, float g, bool m, int l) {
  // 1. If temperature or humidity crosses threshold, turn on fan
  if (t > TEMP_THRESHOLD || h > HUM_THRESHOLD) {
    digitalWrite(FAN_PIN, HIGH);
  } else {
    digitalWrite(FAN_PIN, LOW);
  }

  // 2. If motion is detected AND light is below threshold, turn on a light
  if (m && (l < LIGHT_THRESHOLD)) {
    digitalWrite(LIGHT_PIN, HIGH);
  } else {
    digitalWrite(LIGHT_PIN, LOW);
  }

  // 3. If gas is above threshold, you might trigger an alarm or log special event
  if (g > GAS_THRESHOLD) {
    Serial.println("Warning: Gas sensor reading above threshold!");
  }
}

/**************************************************************************************
 * OPTIONAL: RUN TF-LITE INFERENCE
 **************************************************************************************/
#ifdef ENABLE_TFLITE_INFERENCE
void runTfLiteInference(float t, float h, float g, bool m, int l) {
  // This is placeholder code. Actual TFLite Micro usage requires:
  //  - a model array in model_data.h
  //  - TFLite Micro interpreter init
  //  - preparing input tensor with sensor data
  //  - running interpreter->Invoke()
  //  - reading output for anomaly or classification

  Serial.println("Running placeholder TFLite inference...");
  // e.g., if (inferenceResult indicates anomaly) { do something }
}
#endif

/**************************************************************************************
 * OPTIONAL: SEND DATA TO CLOUD
 **************************************************************************************/
#ifdef ENABLE_CLOUD_LOGGING
void sendToCloudEndpoint(float t, float h, float g, bool m, int l) {
  // Pseudocode to send HTTP request to your cloud or REST API
  // WiFiClient client;
  // if (client.connect("api.example.com", 80)) {
  //   String postData = "temp=" + String(t) + "&hum=" + String(h) + ...
  //   ...
  //   client.print(postData);
  //   ...
  // }
  Serial.println("Data sent to mock cloud endpoint (placeholder).");
}
#endif

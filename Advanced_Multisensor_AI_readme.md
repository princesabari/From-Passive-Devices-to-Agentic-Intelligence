# Advanced Multisensor AI – ESP32 Demo

**File:** `Advanced_Multisensor_AI.ino`

This repository includes a single-file sketch demonstrating multiple sensors, basic rule-based automation, an optional local web server for real-time monitoring, and placeholders for Edge AI inference using TensorFlow Lite Micro. Perfect for students or enthusiasts looking to explore advanced IoT features on a budget-friendly ESP32.

## Features

1. **Sensors**
   - **DHT22**: Temperature/humidity
   - **PIR**: Motion detection
   - **LDR**: Ambient light sensing
   - **MQ-2 (optional)**: Gas detection

2. **Local Web Server**
   - Hosts a real-time dashboard on the ESP32
   - Shows the most recent reading + a ring buffer of the last 10 logs

3. **Rule-Based Automation**
   - Automatically toggles a fan (or relay) when temperature/humidity crosses thresholds
   - Turns on a light if the room is dark and motion is detected
   - Prints alerts for high gas sensor readings

4. **Optional Edge AI** (Placeholder)
   - Define `ENABLE_TFLITE_INFERENCE` to run local TFLite Micro inference
   - Example usage for anomaly detection or classification

5. **Optional Cloud Logging** (Placeholder)
   - Define `ENABLE_CLOUD_LOGGING` to push data to a mock REST endpoint

## Getting Started

### 1. Hardware Setup
- **ESP32 DevKit** (e.g., ESP32-WROOM-32)
- **DHT22** sensor (wired to GPIO4)
- **PIR** sensor (wired to GPIO16)
- **LDR** + resistor in a voltage divider (wired to an ADC pin like GPIO34)
- **MQ-2** gas sensor (optional) to another ADC pin (GPIO35)
- **Fan or LED** (wired to GPIO26 or 27) for automation outputs

### 2. Installation

1. **Clone this repo** or download the `.ino` file.
2. **Arduino IDE** or **PlatformIO**:
   - Install the required libraries:
     - [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)
     - [ESP32 Board definitions](https://github.com/espressif/arduino-esp32)
   - (Optional) For TFLite Micro inference, you’ll need the relevant TensorFlow libraries.
3. Open `Advanced_Multisensor_AI.ino` in your environment.

### 3. Configuration

- Update **`ssid`** and **`password`** for your Wi-Fi.
- Adjust thresholds in the code:
  - `TEMP_THRESHOLD` (e.g., 28°C)
  - `HUM_THRESHOLD` (e.g., 80%)
  - `LIGHT_THRESHOLD` (ADC value for LDR)
  - `GAS_THRESHOLD` (MQ-2 sensor reading)
- Uncomment `#define ENABLE_TFLITE_INFERENCE` if you have a model.
- Uncomment `#define ENABLE_CLOUD_LOGGING` if you have a REST endpoint.

### 4. Usage

1. **Upload** the sketch to your ESP32.
2. Open the **Serial Monitor** at **115200 baud**.
3. Wait for Wi-Fi connection.
4. Check the IP assigned to the ESP32 (e.g., `192.168.1.123`).
5. Open a browser and go to `http://192.168.1.123/`.
6. Observe the live sensor readings and logs.

### 5. Key Functions

- **`automationLogic()`**: Example rule-based approach
- **`updateSensorLog()`**: Stores sensor data in a ring buffer
- **`generateHTMLPage()`**: Builds dynamic HTML for the web server
- **`runTfLiteInference()`** (optional): Placeholder for local AI
- **`sendToCloudEndpoint()`** (optional): Placeholder for cloud logging

### 6. Expanding Further

- **Add more sensors**: Pressure, GPS, or specialized sensors.
- **Refine AI**: Train a custom TFLite model for anomaly detection.
- **MQTT** or **WebSockets**: For real-time message-based communication.
- **Security**: Add basic auth or HTTPS (ESP32 SSL) for secure data.

### 7. Troubleshooting

- If the **DHT** readings show `NaN`, check wiring or ensure correct **DHTTYPE**.
- If **Wi-Fi** fails, verify you’re within router range; add fallback logic if needed.
- For **LDR** values, adjust resistor to get a usable ADC range (10k–100k typically).
- If you see "**Camera init failed**" or such errors, confirm correct pins and board model.

### 8. License
This project is open-source (MIT). Feel free to modify and distribute.

### 9. Contact
For questions or ideas, reach out on [LinkedIn](https://www.linkedin.com/in/sabarinathms/)!

Happy Building! 🎉

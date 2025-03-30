# MQTT Smart Hub – ESP32 Demo

**File:** `MQTT_SmartHub.ino`

This project demonstrates a multi-sensor ESP32 setup that publishes data (temperature, humidity, motion, light levels) to an MQTT broker and subscribes to a command topic to toggle a relay/LED. It also features a simple rule-based system for local decisions if MQTT is unreachable.

---

## Key Features

1. **Wi-Fi Connectivity**  
   - Connects to your local Wi-Fi network.

2. **MQTT Publishing**  
   - Sends sensor data (Temp/Hum, Motion, Light) to specified MQTT topics (e.g., `esp32/sensors/...`).

3. **MQTT Subscription**  
   - Listens on a command topic (e.g., `esp32/cmd`) to remotely switch a relay or LED on/off.

4. **Local Automation Logic**  
   - Simple thresholds for temperature/humidity and ambient light to control a relay, ensuring operation even without MQTT.

5. **Error Handling & Reconnect**  
   - Automatically reconnects to Wi-Fi and MQTT if lost.

---

## Hardware Setup

- **ESP32 DevKit** (e.g., ESP32-WROOM-32)
- **DHT22** sensor (GPIO4) for temperature/humidity
- **PIR** sensor (GPIO16) for motion detection
- **LDR** (GPIO34 analog input) for ambient light
- **Relay/LED** on GPIO26 for external device control

*(Adjust pin assignments in `MQTT_SmartHub.ino` as needed.)*

---

## Installation & Configuration

1. **Clone or Download** this repo (or the `.ino` file).
2. **Arduino IDE** or **PlatformIO**:
   - Install libraries:
     - [ESP32 Board support](https://github.com/espressif/arduino-esp32)
     - [PubSubClient](https://github.com/knolleary/pubsubclient)
     - [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)
3. In **`MQTT_SmartHub.ino`**:
   - Update your **Wi-Fi SSID** and **password**.
   - Set `mqttServer` (e.g., `test.mosquitto.org`) and `mqttPort` (1883 or your broker’s port).
   - (Optional) Adjust thresholds like `TEMP_THRESHOLD`, `HUM_THRESHOLD`, and `LIGHT_THRESHOLD`.

---

## Usage

1. **Upload** the sketch to your ESP32.
2. Open the **Serial Monitor** at **115200 baud** to watch connection status.
3. Once connected, it publishes sensor data at intervals (default 5 seconds).
4. **Subscribe** to the same topics (e.g., `esp32/sensors/temphum`) in an MQTT client to view incoming data.
5. **Send Commands** to the `esp32/cmd` topic:
   - `relay on` to turn relay/LED on
   - `relay off` to turn relay/LED off
6. **Local Logic**: Even if MQTT is unavailable, the ESP32 uses threshold-based rules to drive the relay.

---

## Expanding Further

- **Add More Sensors**: Gas (MQ-2), Pressure (BMP180), etc.
- **Security**: Enable TLS on your MQTT broker or use token-based auth.
- **Integration**: Combine with Node-RED, Home Assistant, or custom dashboards.
- **Data Logging**: Save readings to an SD card or online service.

---

## Troubleshooting

- Check **Serial Monitor** for Wi-Fi or MQTT connection errors.
- If DHT readings show `NaN`, confirm wiring and sensor type (DHT11 vs. DHT22).
- For the PIR or LDR, ensure correct **power** and **pull-down** resistor if needed.
- MQTT connection repeatedly failing? Verify broker IP/hostname, port, and any auth credentials.

---

## License

This project is open-source under the MIT License. Feel free to modify and share.

**Author**: M S Sabarinath
**LinkedIn**: [Sabarinath M S](https://www.linkedin.com/in/sabarinathms/)  

Happy hacking!

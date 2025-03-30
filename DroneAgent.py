"""
File: DroneAgent.py
Assumes a companion computer on each drone
Coordinates coverage tasks via ROS2 or MQTT
"""
import time
import paho.mqtt.client as mqtt

DRONE_ID = "Drone1"
MQTT_BROKER = "192.168.1.210"

def on_message(client, userdata, msg):
    # parse coverage area tasks
    pass

client = mqtt.Client(DRONE_ID)
client.on_connect = lambda c,u,f,rc: client.subscribe("drone/tasks")
client.on_message = on_message
client.connect(MQTT_BROKER, 1883, 60)
client.loop_start()

def main():
    while True:
        # read sensor data (GPS, battery, etc.)
        gps = readGPS()
        battery = readBattery()
        # publish status
        statusPayload = f'{{"gps":{{"lat":{gps.lat},"lng":{gps.lng}}},"battery":{battery}}}'
        client.publish(f"{DRONE_ID}/status", statusPayload)
        
        # run local route planning or check assigned tasks
        # navigate to next area
        time.sleep(5)

if __name__ == "__main__":
    main()

"""
File: BuildingMgmtAgent.py
Raspberry Pi agent controlling HVAC and lighting for a single zone
"""

import time
import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import Adafruit_DHT

# Pins
HVAC_PIN = 18
LIGHT_PIN = 23
DHT_PIN   = 4

# MQTT
BROKER = "192.168.1.200"
ZONE_ID = "Floor3_ZoneA"

GPIO.setmode(GPIO.BCM)
GPIO.setup(HVAC_PIN, GPIO.OUT)
GPIO.setup(LIGHT_PIN, GPIO.OUT)
GPIO.output(HVAC_PIN, GPIO.LOW)
GPIO.output(LIGHT_PIN, GPIO.LOW)

def on_connect(client, userdata, flags, rc):
    client.subscribe("building/commands/#")

def on_message(client, userdata, msg):
    # parse commands or coordination signals
    pass

client = mqtt.Client(ZONE_ID)
client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, 1883, 60)
client.loop_start()

def read_sensors():
    hum, temp = Adafruit_DHT.read_retry(Adafruit_DHT.DHT22, DHT_PIN)
    # you can read CO2 or motion sensors similarly
    return temp, hum

def manage_hvac(temp, hum):
    # simple rule or small ML
    if temp > 26 or hum > 75:
        GPIO.output(HVAC_PIN, GPIO.HIGH)
    else:
        GPIO.output(HVAC_PIN, GPIO.LOW)

def manage_lights(occupancy_detected):
    if occupancy_detected:
        GPIO.output(LIGHT_PIN, GPIO.HIGH)
    else:
        GPIO.output(LIGHT_PIN, GPIO.LOW)

while True:
    temp, hum = read_sensors()
    # occupancy logic (if you have motion sensors, parse them)
    # for simplicity let's say occupant = (temp>20)
    occupant = (temp > 20)
    manage_hvac(temp, hum)
    manage_lights(occupant)

    # publish status
    payload = f'{{"temp":{temp:.1f}, "hum":{hum:.1f}, "occupant":{occupant}}}'
    client.publish("building/status/floor3_zoneA", payload)

    time.sleep(10)

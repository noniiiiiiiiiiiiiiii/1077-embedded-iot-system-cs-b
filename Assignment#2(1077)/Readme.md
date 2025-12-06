# ESP32 IoT Assignment – Webserver & Blynk Cloud

## Overview
This repository contains my IoT assignment based on ESP32 programming.  
The assignment focuses on building a simple temperature and humidity monitoring system using:
- ESP32 Webserver
- Blynk Cloud platform  

The main goal is to understand local webserver working and cloud-based IoT communication.

---

## Hardware Components Used
- ESP32 Dev Board  
- DHT11 Temperature and Humidity Sensor  
- OLED Display  
- Push Button  
- Breadboard  
- Jumper Wires  

---

## Software & Tools
- Arduino IDE / VS Code (PlatformIO)  
- Blynk Cloud  
- Git & GitHub  

---

## Project Files
- `webserver.cpp`  
  ESP32 webserver code to display temperature and humidity on a web page.

- `blynk.cpp`  
  ESP32 code for sending sensor data to Blynk Cloud using virtual pins.

- `README.md`  
  Project documentation file.

---

## ESP32 Webserver Working
The ESP32 first connects to a Wi-Fi network using given credentials.  
After successful connection, it starts a web server on port 80.  
When the ESP32 IP address is opened in a browser, a webpage is displayed showing temperature and humidity values.

A push button is used to take sensor readings, and the same values are also shown on the OLED display.  
The webpage refreshes automatically to show updated readings.

---

## Blynk Cloud Integration
A Blynk template is created in the Blynk Cloud dashboard.  
The ESP32 connects to the cloud using Template ID and Auth Token.  
Temperature and humidity values are sent to Blynk using virtual pins and displayed on the mobile app in real time.

---

## Common Issues Faced
- ESP32 not connecting to Wi-Fi  
- Wrong DHT sensor type selected  
- Incorrect Blynk Template ID or Auth Token  

These issues were fixed by proper configuration and testing.

---

## Conclusion
This assignment helped in understanding how ESP32 works as a webserver and how sensor data can be monitored remotely using Blynk Cloud.  
It provides a simple and practical approach to IoT-based monitoring systems.

---

## Author
Name: **Muhammad Umar Mushtaq**  
Course: Internet of Things and Embedded System

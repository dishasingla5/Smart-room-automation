# Smart Room Automation System

An IoT-based smart room automation system using ESP32 and Arduino for intelligent control of appliances, lighting, and environmental conditions. The system integrates RFID-based access control, automatic lighting using LDR, and cloud monitoring via ThingSpeak.

---

## Features

- RFID-based secure access control
- Automatic day/night detection using LDR
- Curtain automation using servo motors
- Real-time monitoring using ThingSpeak cloud
- LCD display for system status
- RGB LED status indication
- WiFi auto-reconnection handling
- Serial communication between ESP32 and Arduino

---

## System Architecture

### ESP32 Responsibilities
- WiFi connectivity
- ThingSpeak cloud communication
- RFID authentication
- Light sensing using LDR

### Arduino Responsibilities
- Curtain control using servo motors
- LCD display handling
- Appliance control (AC, geyser indicators)

### Communication
- UART serial communication between ESP32 and Arduino

---

## Hardware Components

- ESP32
- Arduino Uno
- MFRC522 RFID module
- LDR (Light Dependent Resistor)
- Servo motors
- 16x2 I2C LCD
- RGB LED
- Relay/LED indicators for appliances

---

## Software and Technologies

- Embedded C/C++
- Arduino IDE
- ThingSpeak IoT platform
- UART communication protocol

---

## Repository Structure

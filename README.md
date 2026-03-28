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

```
smart-room-automation/
│
├── arduino_controller/
│   └── arduino_controller.ino
│
├── esp32_controller/
│   └── esp32_controller.ino
│
├── README.md
└── LICENSE
```

## Working Principle

1. RFID card is scanned using MFRC522 module.
2. If authorized, the room state toggles (ON/OFF).
3. LDR detects ambient light:
   - Bright → Day mode
   - Dark → Night mode
4. ESP32 sends commands to Arduino via serial communication.
5. Arduino controls:
   - Curtain movement
   - LCD display updates
   - Appliance indicators
6. Data is uploaded to ThingSpeak every 15 seconds.

---

## Future Improvements

- MQTT-based communication
- Mobile application integration
- Voice assistant support
- Integration with ROS2 for robotic systems
- Advanced energy monitoring

---

## License

This project is licensed under the MIT License.

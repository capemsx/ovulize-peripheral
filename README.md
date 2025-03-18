# Ovulize Temperature Peripheral

## Overview
This project implements a high-precision temperature measurement peripheral for fertility tracking using an ESP32 and TMP117 temperature sensor. The device communicates with the Ovulize app via Bluetooth Low Energy (BLE).

## Hardware Requirements
- ESP32 development board (Denky32)
- TMP117 high-precision temperature sensor (±0.1°C accuracy)
- Basic wiring/connection components
- Optional: LiPo battery for portability

## Features
- High-precision temperature measurements (±0.1°C / 0.18°F)
- Bluetooth Low Energy connectivity
- Command-based control from mobile app
- Configurable temperature streaming
- Visual status indication via onboard LED
- Low power consumption for extended battery life

## Software Dependencies
Required libraries (automatically installed by PlatformIO):
- [ArduinoBLE](https://www.arduino.cc/reference/en/libraries/arduinoble/)
- [Adafruit TMP117](https://github.com/adafruit/Adafruit_TMP117)
- [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor)

## Connection Diagram
```
ESP32        TMP117
-------      -------
3.3V    -->  VCC
GND     -->  GND
SDA     -->  SDA
SCL     -->  SCL
```

## Building and Flashing

### Using PlatformIO
1. Clone this repository
2. Open the project in VS Code with PlatformIO extension
3. Connect your ESP32 board via USB
4. Click the "Upload" button or run:
```
pio run --target upload
```

### Using Arduino IDE
1. Install required libraries through Library Manager
2. Set board to ESP32 Dev Module
3. Select the correct port
4. Upload the sketch

## BLE Communication Protocol

### Service and Characteristics
- Service UUID: "fff0"
- Data Characteristic UUID: "fff2" (Read, Notify) - Transmits temperature values
- Command Characteristic UUID: "fff1" (Write) - Receives commands from app

### Commands
The device accepts the following string commands:
- `startTemperatureStream`: Begin streaming temperature readings
- `stopTemperatureStream`: End temperature streaming

### Temperature Data Format
Temperature is sent as an integer representing temperature × 100 (in Celsius).
Example: 3725 represents 37.25°C

## Device Operation
1. On power-up, the device initializes the TMP117 sensor and BLE
2. The onboard LED blinks to indicate waiting for connection
3. When connected to the app, it listens for commands
4. When streaming is active, temperature readings are sent every 500ms
5. Streaming automatically stops after 60 seconds if not manually stopped

## Troubleshooting
- If the LED remains on without blinking, check serial output (9600 baud)
- If sensor initialization fails, the device will loop with error message
- For connection issues, ensure BLE is enabled on your mobile device

## License
This project is open source and available under the MIT License.

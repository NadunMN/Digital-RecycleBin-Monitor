# Smart Bin Monitoring System

## Overview
The Smart Bin Monitoring System is an IoT-based project designed to monitor the status of a waste bin in real-time. The system uses an ESP32 microcontroller to collect data from various sensors and provide a web interface for monitoring the bin's status. The project aims to improve waste management by providing timely information about the bin's fill level and lid status.

## Features
- **Real-time Monitoring**: The system provides real-time data on the bin's fill level and lid status.
- **Web Interface**: A user-friendly web interface allows users to monitor the bin's status from any device with a web browser.
- **LED Indicators**: LEDs indicate the bin's status (e.g., lid open/close, 80% full, etc.).
- **Ultrasonic Sensor**: Measures the distance to the waste inside the bin to determine the fill level.
- **LDR Sensor**: Detects whether the bin's lid is open or closed.
- **WiFi Connectivity**: The system connects to a WiFi network to provide remote access to the monitoring interface.

## Hardware Components
- **ESP32 Microcontroller**: The main controller for the system.
- **Ultrasonic Sensor (HC-SR04)**: Measures the distance to the waste inside the bin.
- **LDR (Light Dependent Resistor)**: Detects whether the bin's lid is open or closed.
- **LEDs**: Indicate the bin's status (lid open/close, 80% full, etc.).
- **Resistors and Wires**: For connecting components.

## Software Components
- **Arduino IDE**: Used for programming the ESP32.
- **WiFi Library**: Enables WiFi connectivity.
- **WebServer Library**: Handles HTTP requests and serves the web interface.
- **ESPmDNS Library**: Allows the device to be accessed via a local domain name (e.g., `smartbin.local`).

## Installation and Setup

### Hardware Setup
1. **Connect the Ultrasonic Sensor**:
   - `Trig` pin to GPIO 4
   - `Echo` pin to GPIO 5
   - `VCC` to 5V
   - `GND` to GND

2. **Connect the LDR**:
   - One end to 3.3V
   - The other end to GPIO 34 (analog input) and a 10kΩ resistor to GND.

3. **Connect the LEDs**:
   - `ledPin` (lid status) to GPIO 23
   - `fullLedPin` (80% full) to GPIO 22
   - `statusLed` (server activity) to GPIO 13
   - Each LED should be connected in series with a 220Ω resistor to GND.

### Software Setup
1. **Install Arduino IDE**: Download and install the Arduino IDE from [arduino.cc](https://www.arduino.cc/).

2. **Install ESP32 Board Package**:
   - Open Arduino IDE, go to `File > Preferences`.
   - Add the following URL to the "Additional Boards Manager URLs" field:
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Go to `Tools > Board > Boards Manager`, search for "ESP32", and install the package.

3. **Install Required Libraries**:
   - Go to `Sketch > Include Library > Manage Libraries`.
   - Search for and install the following libraries:
     - `WiFi`
     - `WebServer`
     - `ESPmDNS`

4. **Upload the Code**:
   - Open the provided code in the Arduino IDE.
   - Replace `<Connection_Name>` and `<password>` with your WiFi credentials.
   - Select the correct board (`Tools > Board > ESP32 Dev Module`) and port (`Tools > Port`).
   - Upload the code to the ESP32.

5. **Access the Web Interface**:
   - Once the ESP32 is connected to WiFi, open a web browser and navigate to `http://smartbin.local` or the IP address displayed in the Serial Monitor.
   - The web interface will display the bin's fill level, lid status, and other relevant information.

## Usage
- **Web Interface**: The web interface provides real-time updates on the bin's status. The page automatically refreshes every 5 seconds to display the latest data.
- **LED Indicators**:
  - `ledPin` (GPIO 23): Indicates whether the lid is open (HIGH) or closed (LOW).
  - `fullLedPin` (GPIO 22): Indicates whether the bin is 80% full (HIGH) or not (LOW).
  - `statusLed` (GPIO 13): Indicates server activity (blinks when handling requests).

## API Endpoints
- **`/`**: The root endpoint serves the main web interface.
- **`/status`**: Returns a JSON object with the current status of the bin, including distance, fill percentage, lid status, bin status, and 80% full status.

## Troubleshooting
- **WiFi Connection Issues**: Ensure the WiFi credentials are correct and the ESP32 is within range of the router.
- **Sensor Readings**: Check the wiring of the ultrasonic sensor and LDR. Ensure the sensors are properly connected and functioning.
- **Web Interface Not Loading**: Ensure the ESP32 is connected to the network and the IP address is correct. Check the Serial Monitor for any error messages.

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request with your changes.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments
- Thanks to the Arduino and ESP32 communities for their extensive documentation and support.
- Special thanks to the developers of the libraries used in this project.

## Contact
For any questions or feedback, please open an issue on GitHub or contact the project maintainers.
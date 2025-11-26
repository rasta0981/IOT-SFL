# LoRa Environment Monitoring System Documentation

This folder contains an IoT system built using ESP32 microcontrollers and LoRa wireless modules, designed to collect and monitor environmental sensor data (temperature, humidity, soil moisture), display results locally, transmit them wirelessly, and visualize them on a web dashboard. The codebase includes transmitter and receiver firmware, backend API scripts, and a web frontend.

---

## Contents

- `LoRa_radio.ino`  
  ESP32 firmware (transmitter) for sensor readings and sending data via LoRa.
- `LoRa_radio_receiver.ino`  
  ESP32 firmware (receiver) to process LoRa packets, display on OLED, and push data to a web server.
- `get_data.php`  
  PHP API script to fetch latest sensor data from the MySQL database.
- `index.html`  
  Frontend dashboard UI for monitoring environment data.
- `script.js`  
  JavaScript to update dashboard data via AJAX requests to the API.

---

## Transmitter: LoRa_radio.ino

- **Purpose:**  
  Runs on ESP32; periodically gathers data from the AHT20 temperature/humidity sensor and LM393 soil moisture sensor.  
  Sends sensor payloads to a LoRa receiver; then enters deep sleep to conserve power.
- **Features:**  
  - Reads temperature (°F), humidity (%) and soil moisture (%).
  - Formats sensor data as: `T:<temperature>,H:<humidity>,M:<moisture>`
  - Transmits data using AT commands over Serial2 to the LoRa module.
  - Deep sleep for configurable duration (`TIME_TO_SLEEP`).

- **Key Libraries:**  
  - `Wire.h`, `Adafruit_AHTX0.h` - I2C sensors  
  - HardwareSerial - for LoRa communication

---

## Receiver: LoRa_radio_receiver.ino

- **Purpose:**  
  ESP32-based receiver with OLED display and Wi-Fi capabilities.
- **Features:**  
  - Receives LoRa data packets, parses and displays readings on an SSD1306 OLED.
  - Pushes parsed sensor data to a PHP server (API endpoint) via HTTP POST.
  - Wi-Fi credentials and server settings are configurable.

- **Key Libraries:**  
  - `Adafruit_GFX`, `Adafruit_SSD1306` - OLED display  
  - `WiFi.h`, `HTTPClient.h` - network communication

- **Data Flow:**  
  1. LoRa packet arrives at the receiver.
  2. Device displays latest data on OLED.
  3. POSTs `temperature`, `humidity`, `moisture` to the defined backend server.

---

## Backend API: get_data.php

- **Purpose:**  
  REST-style PHP endpoint serving the latest sensor data to the dashboard frontend.
- **Features:**  
  - Connects to MySQL using provided credentials.
  - Returns the most recent data row (temperature, humidity, moisture, timestamp) from the `aht` table as JSON.
  - Sets CORS headers for cross-origin requests.
- **Config:**  
  Update `$servername`, `$username`, `$password`, `$dbname` to match your DB setup.

---

## Web Dashboard: index.html & script.js

- **Purpose:**  
  A user-friendly dashboard for monitoring the latest sensor data.
- **Features:**  
  - Display current readings for temperature, humidity, and soil moisture.
  - Light, responsive UI using vanilla HTML/CSS/JS.
  - Refresh button to fetch latest values from the backend.
  - AJAX calls to `get_data.php` via `script.js`.
  - Displays update status and error messages to the user.

---

## Quick Start

1. **Hardware**
   - Flash transmitter (`LoRa_radio.ino`) and receiver (`LoRa_radio_receiver.ino`) to separate ESP32 boards.
   - Wire sensors and LoRa modules as per pin definitions in the code.

2. **Backend**
   - Create a MySQL database and table (`aht`) with columns: `temperature`, `humidity`, `moisture`, `datetime`.
   - Configure XAMPP (or similar stack) and place `get_data.php` appropriately.
   - Ensure receiver points to server IP in its configuration.

3. **Frontend**
   - Place `index.html`, `script.js`, and ensure they point to the correct API endpoint.
   - Open `index.html` in your browser for real-time readings.

---

## Data Format

- **Transmitted packet (example):**  
  `AT+SEND=2,17,T:75.2,H:55.1,M:45\r\n`
- **Backend API response (JSON):**
  ```json
  {
    "status": "success",
    "message": "Data retrieved successfully",
    "data": {
      "temperature": 75.2,
      "humidity": 55.1,
      "moisture": 45,
      "timestamp": "2025-11-26 12:00:00"
    }
  }
  ```

---

## Notes

- **Calibration:**  
  Soil moisture sensor calibration parameters (min/max) must be tuned for your hardware.
- **Security:**  
  Update Wi-Fi and DB credentials before deployment.
- **Extensibility:**  
  You can adapt the firmware for other sensors or expand database dashboard queries.

---

## References
- [Adafruit AHTX0](https://github.com/adafruit/Adafruit_AHTX0)
- [ESP32 LoRa communication](https://randomnerdtutorials.com/esp32-lora-rfm95/)
- [SSD1306 OLED](https://github.com/adafruit/Adafruit_SSD1306)
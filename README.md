# IOT-SFL
SkyFire LABs demo projects.

Useful code to quickly stand up a fully functional IOT nodes and interfaces.

___
## Devices
ESP32_Water Pump webserver 💦 `(RA-1) - Waterpump_webserver.ino`
- Esp32 connected water pump powered by solar and connected to a relay module for on/off operation.
<br/>

ESP32_Soil Sensor webserver 🌡️ `(RA-2) - Soil_webserver.ino`
- Esp32 connected soil sensor powered by solar. Temeparture readings are displayed on central hub and transmitted wirelessy via ESP_NOW protocal

## Esp_now

Receiver_espnow.ino 
- sample esp_now code used to receive DHT11 sensor data using embedded deep sleep function within ESP32
 
Sender_espnow.ino
- Sample code to send DHT11 sensor values for Humidity and temperature to listener ESP32 

___

## LoRa
- (RA-3) Environment sensors used to capture temperature, humidity and soil moisture content and transmit via LoRa(Long range)


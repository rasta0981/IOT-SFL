// Transmitter code for ESP32 with Deep Sleep
#include <Wire.h> // Required for I2C communication
#include <Adafruit_AHTX0.h> // Library for AHT20 sensor

#define LORA_RX_PIN 16 // Connect to LoRa TX pin
#define LORA_TX_PIN 17 // Connect to LoRa RX pin
#define TX_ADDRESS "2" // The address of the receiver module
#define SOIL_MOISTURE_PIN 4 // Connect the analog output (AO) of the LM393 to GPIO 4

// Create a HardwareSerial instance for the LoRa module using Serial2
HardwareSerial loraSerial(2); 
Adafruit_AHTX0 aht; // Create AHT20 sensor object

// Time the ESP32 should sleep in microseconds (1 hour = 3600 seconds)
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  12        /* Time ESP32 will go to sleep (in seconds) */

// Function to convert raw analog reading to a human-readable percentage
int rawToPercent(int rawValue, int minValue, int maxValue) {
    int percentage = map(rawValue, maxValue, minValue, 0, 100); 
    return constrain(percentage, 0, 100);
}

void setup() {
  // Initialize USB Serial for debugging
  Serial.begin(115200);
  
  // Suppress NACK failure warnings 
  esp_log_level_set("i2c.master", ESP_LOG_NONE);
  
  // Initialize hardware serial for LoRa module (Serial2)
  loraSerial.begin(115200, SERIAL_8N1, LORA_RX_PIN, LORA_TX_PIN);  
  
  // Initialize I2C communication for the AHT sensor
  Wire.begin(21, 22);
  if (! aht.begin()) {
    Serial.println("Could not find AHT sensor? Check wiring");
    while (1) delay(10); 
  }

  // Set the soil moisture pin as an input
  pinMode(SOIL_MOISTURE_PIN, INPUT);

  delay(1000);
  Serial.println("Transmitter Ready (Send Only)");
}

void loop() {
  // --- Read AHT Sensor Data ---
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp); 

  // --- Read Soil Moisture Data ---
  int rawMoisture = analogRead(SOIL_MOISTURE_PIN);
  int moisturePercent = rawToPercent(rawMoisture, 1800, 4095); 

  // --- Format Data ---
  float tempFahrenheit = (temp.temperature * 1.8) + 32;   
  String temperatureStr = String(tempFahrenheit, 1); 
  String humidityStr = String(humidity.relative_humidity, 1);
  String moistureStr = String(moisturePercent);

  // Message format: "T:77.7,H:55.2,M:45"
  String sensorData = "T:" + temperatureStr + ",H:" + humidityStr + ",M:" + moistureStr; 
  
  // --- LoRa Transmission (Transmit Only) ---
  String commandToSend = "AT+SEND=" + String(TX_ADDRESS) + "," + String(sensorData.length()) + "," + sensorData;
  commandToSend += "\r\n"; 

  loraSerial.print(commandToSend); 
  
  Serial.print("Sent: ");
  Serial.println(sensorData);

  // Ensure all Serial data is sent before sleeping
  Serial.flush(); 

  // --- Deep Sleep Implementation ---
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.print("Sleeping for ");
  Serial.print(TIME_TO_SLEEP);
  Serial.println(" seconds...");
  
  esp_deep_sleep_start();
}

// Transmitter code for ESP32 with Deep Sleep
#include <Wire.h> // Required for I2C communication
#include <Adafruit_AHTX0.h> // Library for AHT20 sensor

#define LORA_RX_PIN 16 // Connect to LoRa TX pin
#define LORA_TX_PIN 17 // Connect to LoRa RX pin
#define TX_ADDRESS "2" // The address of the receiver module
#define SOIL_MOISTURE_PIN 18 // Connect the analog output (AO) of the LM393 to GPIO 34 (an ADC pin)

// Create a HardwareSerial instance for the LoRa module using Serial2
HardwareSerial loraSerial(2); 
Adafruit_AHTX0 aht; // Create AHT20 sensor object

// Time the ESP32 should sleep in microseconds (30 minutes = 1800 seconds)
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  120        /* Time ESP32 will go to sleep (in seconds) */

String mymessage;

// Function to convert Celsius to Fahrenheit
float convertCtoF(float c) {
  return (c * 1.8) + 32;
}

// Function to convert raw analog reading to a human-readable percentage
// Calibrate these min/max values based on your sensor in dry air and a glass of water
int rawToPercent(int rawValue, int minValue, int maxValue) {
    // Map the raw reading to a percentage, inverted (higher raw value = drier soil)
    int percentage = map(rawValue, maxValue, minValue, 0, 100); 
    // Constrain the value between 0 and 100
    return constrain(percentage, 0, 100);
}

void setup() {
  // Initialize USB Serial for debugging (connected via USB port to PC)
  Serial.begin(115200);      
  
  // Initialize hardware serial for LoRa module (Serial2)
  loraSerial.begin(115200, SERIAL_8N1, LORA_RX_PIN, LORA_TX_PIN);  
  
  // Initialize I2C communication for the AHT sensor
  if (! aht.begin()) {
    Serial.println("Could not find AHT sensor? Check wiring");
    while (1) delay(10); // Halt if sensor is not found
  }

  // Set the soil moisture pin as an input
  pinMode(SOIL_MOISTURE_PIN, INPUT);

  delay(1000);
  Serial.println("Transmitter Ready on ESP32 with AHT/LM393 Sensors");
  Serial.print("Going to sleep for ");
  Serial.print(TIME_TO_SLEEP);
  Serial.println(" seconds.");
}

void loop() {
  // --- Read AHT Sensor Data ---
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp); 

  // --- Read Soil Moisture Data ---
  int rawMoisture = analogRead(SOIL_MOISTURE_PIN);
  // Example calibration values: adjust these based on your specific sensor performance
  int moisturePercent = rawToPercent(rawMoisture, 1800, 4095); 

  // --- Format Data ---
  float tempFahrenheit = (temp.temperature * 1.8) + 32;   
  String temperatureStr = String(tempFahrenheit, 1); 
  String humidityStr = String(humidity.relative_humidity, 1);
  String moistureStr = String(moisturePercent);

  // Message format: "T:77.7,H:55.2,M:45" (Temp F, Humidity %, Moisture %)
  String sensorData = "T:" + temperatureStr + ",H:" + humidityStr + ",M:" + moistureStr; 
  
  
  // --- LoRa Transmission ---
  // Construct the AT command for the LoRa module
  // Format: AT+SEND=[Address],[Length],[Data]
  String commandToSend = "AT+SEND=" + String(TX_ADDRESS) + "," + String(sensorData.length()) + "," + sensorData;
  commandToSend += "\r\n"; // Append CR/LF

  loraSerial.print(commandToSend); 
  
  Serial.print("Sent: ");
  Serial.println(commandToSend);
  Serial.print("Raw Moisture Reading: ");
  Serial.println(rawMoisture);

  // Read response from Lora module if available (optional)
  delay(100); 
  if (loraSerial.available()) {
    Serial.print("LoRa response: ");
    while(loraSerial.available()) {
      Serial.write(loraSerial.read());
    }
    Serial.println(); 
  }

  // Ensure all Serial data is sent over USB before sleeping
  Serial.flush(); 

  // --- Deep Sleep Implementation ---
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Entering deep sleep now");
  
  esp_deep_sleep_start();

  // Code stops here until next wake-up/reboot
}

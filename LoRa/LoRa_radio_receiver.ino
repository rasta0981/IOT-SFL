// Receiver code for ESP32 with OLED support and XAMPP Integration

#include <SPI.h>
#include <Wire.h> // Required for I2C communication
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // Library for the OLED display
#include <WiFi.h> // Required for ESP32 Wi-Fi connectivity
#include <HTTPClient.h> // Required for making HTTP requests

#define LORA_RX_PIN 16 // Connect to LoRa TX pin
#define LORA_TX_PIN 17 // Connect to LoRa RX pin
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels (adjust to 32 if you have a 128x32 display)

// --- WiFi and Server Configuration ---
const char* ssid = "*******";         // Your WiFi network SSID
const char* password = "*******!"; // Your WiFi network password
const char* serverName = "http://localhost/aht/aht.php"; // URL of your PHP script
//const char* serverName = "http://localhost/aht/get_data.php"; // URL of your PHP 

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Create a HardwareSerial instance for the LoRa module using Serial2
HardwareSerial loraSerial(2); 

void setup() {
  Serial.begin(115200);      
  loraSerial.begin(115200, SERIAL_8N1, LORA_RX_PIN, LORA_TX_PIN);  

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("LoRa Receiver");
  display.println("Connecting to WiFi...");
  display.display(); 
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi Connected!");
  display.println("Waiting for data...");
  display.display();
}

void loop() {
  if (loraSerial.available()) {
    String incomingMessage = "";
    while(loraSerial.available()) {
      char incomingChar = loraSerial.read();
      incomingMessage += incomingChar;
    }
    
    Serial.print("Received raw message: ");
    Serial.println(incomingMessage);

    // Parse data, update OLED, and send to XAMPP server
    sendToServer(incomingMessage);
    updateOLED(incomingMessage);
  }
  
  delay(100); 
}

// Function to parse the message and extract data values
void parseSensorData(String rawMessage, String &tempStr, String &humStr, String &moistureStr) {
    // Expected raw message format: +RCV=2,17,T:25.4,H:55.2,M:45,-30\r\n
    
    // Find where the actual sensor data starts (after the third comma in the raw string)
    int firstComma = rawMessage.indexOf(',');
    int secondComma = rawMessage.indexOf(',', firstComma + 1);
    int thirdComma = rawMessage.indexOf(',', secondComma + 1);

    // Isolate the data payload section (e.g., T:25.4,H:55.2,M:45)
    String dataString = rawMessage.substring(secondComma + 1, rawMessage.lastIndexOf(','));

    // Extract T, H, and M values
    int tIndex = dataString.indexOf("T:");
    int hIndex = dataString.indexOf("H:");
    int mIndex = dataString.indexOf("M:");
    
    tempStr = dataString.substring(tIndex + 2, hIndex - 1);
    humStr = dataString.substring(hIndex + 2, mIndex - 1);
    moistureStr = dataString.substring(mIndex + 7);
}


// Function to parse the message, send to server
void sendToServer(String rawMessage) {
    String tempStr, humStr, moistureStr;
    parseSensorData(rawMessage, tempStr, humStr, moistureStr);

    // --- HTTP POST Request to XAMPP Server ---
    if(WiFi.status() == WL_CONNECTED){
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Create the POST data string (now includes 'moisture')
      String httpRequestData = "temperature=" + tempStr + "&humidity=" + humStr + "&moisture=" + moistureStr;
      
      Serial.print("Sending to XAMPP: ");
      Serial.println(httpRequestData);

      int httpResponseCode = http.POST(httpRequestData);

      if (httpResponseCode > 0) {
        Serial.printf("[HTTP] POST... code: %d\n", httpResponseCode);
        String response = http.getString();
        Serial.println(response);
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
      }
      http.end();
    } else {
      Serial.println("WiFi Disconnected, cannot send data.");
    }
}

// Function to parse the message and display cleanly on the OLED
void updateOLED(String rawMessage) {
    String tempStr, humStr, moistureStr;
    parseSensorData(rawMessage, tempStr, humStr, moistureStr);

    // Update the OLED display
    display.clearDisplay();
    display.setTextSize(1); // Small text for header
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("SKYFIRE LABS inc.");
    display.println("--------------------");


    display.setCursor(0, 20);
    display.print("Temp: ");
    display.setTextSize(1); // Larger text for value
    display.print(tempStr);
    display.setTextSize(1); // Back to small for units
    display.cp437(true); // Enable special characters (degree symbol)
    display.write(248); // Print degree symbol
    display.print("F");

    display.setCursor(0, 35);
    display.print("Hum:  ");
    display.setTextSize(1);
    display.print(humStr);
    display.setTextSize(1);
    display.print("%");

    display.setCursor(0, 48);
    display.print("Moist: ");
    display.setTextSize(1);
    display.print(moistureStr);
    display.setTextSize(1);
    display.print("%");
    
    display.display(); // Push the buffer to the screen
}

/*
  ESP-NOW Demo - Transmit
  esp-now-demo-xmit.ino
  Sends data to Responder
  
  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include Libraries
#include <esp_now.h>
#include <WiFi.h>
#include <DHT11.h>

// Variables for test data
DHT11 dht11(19);
//int int_value;
//float float_value;
//bool bool_value = true;

//Define TIme to SLeep
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  10  // Sleep for 10 seconds

// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0x**, 0x**, 0x**, 0x**, 0x**, 0x**};

// Define a data structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
 // bool d;
} struct_message;

// Create a structured object
struct_message myData;

// Peer info
esp_now_peer_info_t peerInfo;

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {

  pinMode(19, OUTPUT); 
  // Set up Serial Monitor
  Serial.begin(115200);
 
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {

  // Create test data

    int temperature = 0;
    int humidity = 0;

    // Attempt to read the temperature and humidity values from the DHT11 sensor.
    int result1 = dht11.readTemperatureHumidity(temperature, humidity);
    int fresult = temperature * 9/5 + 32;
    

    // Check the results of the readings.
    // If the reading is successful, print the temperature and humidity values.
    // If there are errors, print the appropriate error messages.
    if (result1 == 0) {
        Serial.print("Temperature: ");
        Serial.print(fresult);
        Serial.print(" °F\tHumidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(result1));
    }

  // Generate a random integer
  //int_value = random(1,20);

  // Use integer to make a new float
  // float_value = 1.3 * int_value;

  // Invert the boolean value
  // bool_value = !bool_value;
  
  // Format structured data
  strcpy(myData.a, "Central Hub!");
  myData.b = fresult;
  myData.c = humidity;
 // myData.d = bool_value;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  // Prepare for sleep
  // Deep Sleep function to allow bettewr power consumption when running form battery
  Serial.flush(); // Ensure outgoing serial data is sent
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); //Wake up after 10 seconds.
  esp_deep_sleep_start();
}

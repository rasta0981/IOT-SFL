#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// Replace with your network credentials
const char* ssid = "<REPLACE WITH WIFI>";         /*Enter Your SSID*/
const char* password = "<REPLACE WITH WIFI PASSWORD>"; /*Enter Your Password*/

WebServer server(80);

const int sensorPin = 36;  // Replace with your sensor's analog pin

// Function to read soil moisture and convert to percentage
int getSoilMoisture() {
  int sensorValue = analogRead(sensorPin);
  // Calibrate these values based on your sensor and soil type
  int dryValue = 4095; // Sensor value when dry (e.g., no moisture)
  int wetValue = 1320; // Sensor value when wet (e.g., moisture)

  // Ensure sensorValue is within the expected range
  sensorValue = constrain(sensorValue, wetValue, dryValue);

  // Calculate percentage
  int moisturePercentage = map(sensorValue, wetValue, dryValue, 100, 0);
  return constrain(moisturePercentage, 0, 100);
}

// Web page content
const char index_html[] PROGMEM = R"raw(
<!DOCTYPE html>
<html>
<head>
  <title>Knowles Soil Moisture</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: sans-serif; text-align: center; background-color: #4B5320;}
    .moisture-display { font-size: 2em; margin-top: 20px; }
  </style>
</head>
<body>
  <h1>Knowles_Tomato Soil Sensor</h1>
  <div class="moisture-display">
    Moisture: <span id="moistureValue"></span>%
  </div>
  <script>
    setInterval(function() {
      getData();
    }, 2000); // Update every 2 seconds

    function getData() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("moistureValue").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "/moisture", true);
      xhttp.send();
    }
  </script>
</body>
</html>
)raw";

// Handle the root URL ("/")
void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

// Handle the "/moisture" URL (returns the moisture value)
void handleMoisture() {
  int moisture = getSoilMoisture();
  server.send(200, "text/plain", String(moisture));
}

// Handle any other request (e.g., 404)
void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  // Define the routes
  server.on("/", handleRoot);
  server.on("/moisture", handleMoisture);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

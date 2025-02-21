#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// Network credentials
const char *ssid = "<Connection_Name>";
const char *password = "<password>";

// Server instance
WebServer server(80);

// Pin definitions
const int ldrPin = 34;      // LDR analog input pin
const int ledPin = 23;      // LED pin (for lid open/close indication)
const int fullLedPin = 22;  // Additional LED for 80% full indication
const int trigPin = 4;      // Ultrasonic Trig pin
const int echoPin = 5;      // Ultrasonic Echo pin
const int statusLed = 13;   // Status LED pin

// Bin configuration
const int maxHeight = 90;       // Maximum height (1 meter = 90 cm)
const int binThreshold = 72;    // 80% of maxHeight (80% of 90 = 72 cm)
const int eightyPercentFull = 20;  // 80% of bin's full capacity (in cm)
int distance = 0;

// Function declarations
String getHTML(int distance, String lidStatus, String binStatus, String fullLedStatus, int fillPercentage);
int getDistance();
void handleRoot();
void handleNotFound();
void setupWiFi();
void setupServer();

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  
  // Configure pins
  pinMode(ledPin, OUTPUT);
  pinMode(fullLedPin, OUTPUT);
  pinMode(statusLed, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Initialize all LEDs to OFF
  digitalWrite(ledPin, LOW);
  digitalWrite(fullLedPin, LOW);
  digitalWrite(statusLed, LOW);
  
  // Setup WiFi and Server
  setupWiFi();
  setupServer();
}

void loop() {
  server.handleClient();
  
  // Read sensors once
  int ldrValue = analogRead(ldrPin);
  distance = getDistance();
  
  // Update LED states
  digitalWrite(ledPin, ldrValue > 500 ? HIGH : LOW);  // Lid status
  digitalWrite(fullLedPin, distance <= eightyPercentFull ? HIGH : LOW);  // Bin status
  
  delay(10);  // Small delay for stability
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print("...");
    timeout++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    if (MDNS.begin("smartbin")) {
      Serial.println("MDNS responder started at smartbin.local");
    }
  } else {
    Serial.println("\nFailed to connect to WiFi. Check credentials.");
  }
}

void setupServer() {
  // Define server routes
  server.on("/", HTTP_GET, handleRoot);
  
  server.on("/status", HTTP_GET, []() {
    int ldrValue = analogRead(ldrPin);
    distance = getDistance();
    
    String json = "{";
    json += "\"distance\":" + String(distance) + ","; // Actual distance
    json += "\"fillPercentage\":" + String(getFillPercentage()) + ","; // Fill percentage
    json += "\"lid\":\"" + String(ldrValue > 500 ? "Open" : "Closed") + "\","; 
    json += "\"binStatus\":\"" + String(distance <= binThreshold ? "Full" : "Not Full") + "\","; 
    json += "\"eightyPercent\":\"" + String(distance <= eightyPercentFull ? "Yes" : "No") + "\"";
    json += "}";
    
    server.send(200, "application/json", json);
  });
  
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void handleRoot() {
  digitalWrite(statusLed, HIGH);  // Indicate server activity
  
  int ldrValue = analogRead(ldrPin);
  distance = getDistance();
  
  String binStatus = (distance <= binThreshold) ? "Full" : "Not Full";
  String fullLedStatus = (distance <= eightyPercentFull) ? "ON" : "OFF";
  String lidStatus = (ldrValue > 500) ? "Open" : "Closed";
  int fillPercentage = getFillPercentage();
  
  String html = getHTML(distance, lidStatus, binStatus, fullLedStatus, fillPercentage);
  server.send(200, "text/html", html);
  
  digitalWrite(statusLed, LOW);
}

void handleNotFound() {
  digitalWrite(statusLed, HIGH);
  
  String message = "Error 404\n\n";
  message += "URI: " + server.uri() + "\n";
  message += "Method: " + String(server.method() == HTTP_GET ? "GET" : "POST") + "\n";
  message += "Arguments: " + String(server.args()) + "\n";
  
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, "text/plain", message);
  digitalWrite(statusLed, LOW);
}

String getHTML(int distance, String lidStatus, String binStatus, String fullLedStatus, int fillPercentage) {
  String html = "<!DOCTYPE HTML><html>";
  html += "<head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Smart Bin Monitor</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; padding: 20px; background-color: #f0f0f0; margin:auto; height:100vh; display:flex; align-items:center; justify-content:center; }";
  html += "h1 { color: #333; font-size:30px }";
  html += ".container { width: 600px; margin: 0 auto; background-color: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }";
  html += ".value { font-size: 1.5em; color: #4CAF50; font-weight: bold; }";
  html += ".status { margin: 20px 0; padding: 15px; border-radius: 5px; background-color: #f8f8f8; }";
  html += "</style>";
  html += "</head><body>";
  html += "<div class='container'>";
  html += "<h1>Smart Bin Monitor</h1>";
  html += "<div class='status'>";
  html += "<p>Fill Percentage: <span class='value'>" + String(fillPercentage) + "%</span></p>";
  html += "<p>Lid Status: <span class='value'>" + lidStatus + "</span></p>";
  html += "<p>Bin Status: <span class='value'>" + binStatus + "</span></p>";
  html += "</div>";
  html += "</div>";
  html += "<script>";
  html += "setInterval(function() {";
  html += "  fetch('/status')";
  html += "    .then(response => response.json())";
  html += "    .then(data => {";
  html += "      document.querySelectorAll('.value')[0].textContent = data.fillPercentage + '%';";
  html += "      document.querySelectorAll('.value')[1].textContent = data.lid;"; 
  html += "      document.querySelectorAll('.value')[2].textContent = data.binStatus;"; 
  html += "    });";
  html += "}, 5000);"; // Update every 5 seconds
  html += "</script>";
  html += "</body></html>";
  return html;
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  
  if (duration == 0) {
    return 999; // Out of range or error
  }
  
  int distance = duration * 0.034 / 2;
  
  return (distance > 400) ? 999 : distance;  // Consider distance > 400 cm as an error
}

int getFillPercentage() {
  // Fill percentage calculation (100 - percentage of remaining height)
  return max(0, 100 - (distance * 100 / maxHeight));
}

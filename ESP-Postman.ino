#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char *SSID = "rpiwifi";
const char *PWD = "majeed12";

WebServer server(80);

// Macro for the temperature value
#define TEMPERATURE_VALUE 25.0  // Example: 25°C

// Variable to store the pressure value
float pressure = 1013.25;  // Default pressure value in hPa

StaticJsonDocument<250> jsonDocument;
char buffer[250];

void setup_routing() {
  server.on("/temperature", getTemperature);
  server.on("/pressure", HTTP_POST, handlePost);  // Route to set pressure value
  server.begin();
}

void create_json(const char *tag, float value, const char *unit) {
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}

void getTemperature() {
  Serial.println("Get temperature");
  create_json("temperature", TEMPERATURE_VALUE, "°C");
  server.send(200, "application/json", buffer);
}

void handlePost() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body not found");
    return;
  }

  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);

  if (jsonDocument.containsKey("pressure")) {
  pressure = jsonDocument["pressure"];
  Serial.print("Updated pressure: ");
  Serial.println(pressure);
  server.send(200, "application/json", "{\"status\":\"pressure updated\"}");
} else {
  server.send(400, "application/json", "{\"error\":\"missing pressure key\"}");
}
}

void setup() {
  Serial.begin(115200);

  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.print("Connected! IP Address: ");
  Serial.println(WiFi.localIP());
  setup_routing();
}

void loop() {
  server.handleClient();
}

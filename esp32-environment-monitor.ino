#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "DHTesp.h"
#include "secrets.h"

#define DHT_PIN 27
#define MQ_D0   33

DHTesp dht;
WebServer server(80);

// Latest sensor values, updated in loop().
float curTemp = NAN;
float curHum  = NAN;
bool  curGas  = false;

void handleLatest() {
  StaticJsonDocument<128> doc;
  doc["temperature"] = curTemp;
  doc["humidity"] = curHum;
  doc["gas"] = curGas; // true = gas detected

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.setup(DHT_PIN, DHTesp::DHT11);
  pinMode(MQ_D0, INPUT);

  // Connect to Wi-Fi using credentials from secrets.h.
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // HTTP endpoints.
  server.on("/latest", HTTP_GET, handleLatest);
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "ESP32 OK. Open /latest for JSON.");
  });
  server.begin();

  Serial.println("DHT + MQ-135 (D0) started. HTTP server started.");
}

void loop() {
  // Read latest DHT11 values.
  TempAndHumidity th = dht.getTempAndHumidity();
  curTemp = th.temperature;
  curHum  = th.humidity;

  curGas = (digitalRead(MQ_D0) == LOW); // MQ-135 D0 is active LOW.

  // Keep Serial Monitor output for debugging.
  Serial.print("T=");
  Serial.print(curTemp);
  Serial.print(" C | H=");
  Serial.print(curHum);
  Serial.print(" % | MQ=");
  Serial.println(curGas ? "GAZ" : "OK");

  // Serve HTTP requests.
  server.handleClient();

  delay(2500);
}

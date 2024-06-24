#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Ticker.h>

const char* ssid = "AliSK";
const char* password = "87654321";
const char* serverName = "https://erfann313.pythonanywhere.com/api/v1/led/ping/";

const int ledPin = 33;

String status = "OFF";
long timeOn = 0;
long timeOff = 0;
Ticker ticker;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  ticker.attach(1, checkServer);
}

void loop() {
  unsigned long currentMillis = millis();
  if (status == "ON") {
    digitalWrite(ledPin, LOW);
  } else if (status == "OFF") {
    digitalWrite(ledPin, HIGH);
  } else if (status == "Scheduled") {
    if (currentMillis - previousMillis >= timeOn) {
      previousMillis = currentMillis;
      digitalWrite(ledPin, LOW);
      delay(timeOn);
      digitalWrite(ledPin, HIGH);
      delay(timeOff);
    }
  }
}

void checkServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    int httpCode = http.GET();
    if (httpCode == 200) {
      String payload = http.getString();
      Serial.println(payload);
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      status = doc["status"].as<String>();
      if (status == "Scheduled") {
        timeOn = doc["scheduling"]["time_on_ms"];
        timeOff = doc["scheduling"]["time_off_ms"];
      }
    } else {
      Serial.println("Error on HTTP request");
    }
    http.end();
  }
}

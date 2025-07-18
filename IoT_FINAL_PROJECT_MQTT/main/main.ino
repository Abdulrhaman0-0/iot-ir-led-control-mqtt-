/************************************************************
 * ESP8266 / ESP32 MQTT-Controlled Relay & IR Sensor System *
 * 
 * Description:
 * This code connects an ESP8266 or ESP32 to Wi-Fi and MQTT,
 * subscribes to a topic to control a relay (or LED), and
 * publishes IR sensor status periodically.
 * 
 * To use with ESP32:
 * - Change `#include <ESP8266WiFi.h>` to `#include <WiFi.h>`
 * - Replace GPIO names (D5/D6) with actual GPIO numbers (e.g., 14, 12)
 ************************************************************/

// ==========================
// Wi-Fi Configuration
// ==========================
#define ssid "aboud"
#define pass "12345678"

// ==========================
// MQTT Configuration
// ==========================
const char* broker = "10.252.0.156";       // MQTT Broker IP (e.g., Raspberry Pi)
const int port = 1883;
const char* topicSub = "/sectorb5/ABDO/led";      // Topic to receive control commands
const char* topicPub = "/sectorb5/ABDO/irsensor"; // Topic to publish IR sensor status

// ==========================
// Hardware Pin Definitions
// ==========================
// NOTE: When using a Relay instead of LED, ensure relay module is active HIGH or LOW accordingly.
#define RELAY_PIN D6         // Pin connected to relay (or LED for testing)
#define IR_SENSOR_PIN D5     // Pin connected to IR sensor output

// ==========================
// Libraries
// ==========================
// For ESP32: Replace with <WiFi.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ==========================
// Global Variables
// ==========================
WiFiClient wifiClient;
PubSubClient client(wifiClient);
char Array_Payload[10] = {};  // Buffer to hold incoming MQTT payloads

// ==========================
// MQTT Callback Function
// ==========================
void callback(char* topic, byte* payload, unsigned int length) {
  // Copy payload to buffer and null-terminate
  for (unsigned int i = 0; i < length; i++) {
    Array_Payload[i] = (char)payload[i];
  }
  Array_Payload[length] = '\0';

  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(Array_Payload);

  // Compare received message and toggle relay
  if (strcmp(Array_Payload, "On") == 0) {
    digitalWrite(RELAY_PIN, HIGH); // For active HIGH relay
    Serial.println("Relay turned ON");
  } else if (strcmp(Array_Payload, "Off") == 0) {
    digitalWrite(RELAY_PIN, LOW);  // For active HIGH relay
    Serial.println("Relay turned OFF");
  }
}

// ==========================
// Setup Function
// ==========================
void setup() {
  Serial.begin(9600);

  // Configure GPIO modes
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  digitalWrite(RELAY_PIN, LOW); // Ensure relay starts OFF

  // Wi-Fi Connection
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    tries++;
    if (tries > 20) {
      Serial.println("\n❌ Failed to connect to WiFi.");
      return;
    }
  }

  Serial.println("\n✅ WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure MQTT
  client.setServer(broker, port);
  client.setCallback(callback);

  // Connect to MQTT broker
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESPClient")) {
      Serial.println("✅ Connected to MQTT broker!");
      client.subscribe(topicSub);
    } else {
      Serial.print("❌ Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

// ==========================
// Main Loop
// ==========================
unsigned long lastSent = 0;

void loop() {
  client.loop();  // Keep MQTT connection alive

  // Reconnect if connection drops (good practice)
  if (!client.connected()) {
    while (!client.connected()) {
      Serial.println("Reconnecting to MQTT broker...");
      if (client.connect("ESPClient")) {
        Serial.println("✅ Reconnected!");
        client.subscribe(topicSub);
      } else {
        Serial.print("❌ Failed, rc=");
        Serial.print(client.state());
        delay(2000);
      }
    }
  }

  // Periodically publish IR sensor status
  if (millis() - lastSent > 1000) {
    lastSent = millis();

    int sensorState = digitalRead(IR_SENSOR_PIN);
    const char* status = (sensorState == HIGH) ? "CLEAR" : "OBSTACLE";

    if (client.publish(topicPub, status)) {
      Serial.print("Published: ");
      Serial.println(status);
    } else {
      Serial.println("❌ Failed to publish IR status");
    }
  }

  delay(10); // Slight delay to reduce CPU usage
}

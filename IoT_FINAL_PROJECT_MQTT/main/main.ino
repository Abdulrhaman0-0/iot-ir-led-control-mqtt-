#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ==========================
// Wi-Fi Configuration
// ==========================
#define ssid "aboud"
#define pass "12345678"

// ==========================
// MQTT Configuration
// ==========================
const char* broker = "10.252.0.156";       // Raspberry Pi IP
const int port = 1883;
const char* topicSub = "/sectorb5/ABDO/led";      // Topic to receive control commands
const char* topicPub = "/sectorb5/ABDO/irsensor"; // Topic to publish IR sensor status

// ==========================
// Hardware Pin Definitions
// ==========================
#define RELAY_PIN D6         // GPIO for LED (or relay if used)
#define IR_SENSOR_PIN D5     // GPIO connected to IR sensor output

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
  // Copy payload to buffer and terminate with null character
  for (unsigned int i = 0; i < length; i++) {
    Array_Payload[i] = (char)payload[i];
  }
  Array_Payload[length] = '\0';

  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(Array_Payload);

  // Compare received message and toggle output pin
  if (strcmp(Array_Payload, "On") == 0) {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("LED turned ON");
  } else if (strcmp(Array_Payload, "Off") == 0) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("LED turned OFF");
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
  digitalWrite(RELAY_PIN, LOW); // Ensure output is initially off

  // Connect to Wi-Fi network
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

  // Configure MQTT server and callback
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

  // Periodically publish IR sensor state
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
}
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

String wifiSsid = "Robert_Bosch_AP02";
String wifiPass = "BoschCoSpace";
// String mqttHost = "mqtt.eclipse.org";
String mqttHost  = "test.mosquitto.org";
String mqttDeviceName = "BOSCH_0101";
String mqttTopic = mqttDeviceName + "/unlock";

int lockOpenPin = 22;
int lockClosePin = 23;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void callback(char* topic, byte* message, unsigned int length);
void reconnect();
void wifiReconnect();

void setup() {
    pinMode(lockOpenPin, OUTPUT);
    pinMode(lockClosePin, OUTPUT);
    pinMode(BUILTIN_LED, OUTPUT);

    digitalWrite(lockOpenPin, HIGH);
    digitalWrite(lockClosePin, HIGH);
    digitalWrite(BUILTIN_LED, HIGH);

    Serial.begin(115200);

    wifiReconnect();

    mqttClient.setServer(mqttHost.c_str(), 1883);
    mqttClient.setCallback(callback);
    Serial.println("end of setup()");
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        wifiReconnect();
    }
    else {
        if (!mqttClient.connected()) {
            Serial.println("mqtt disconnected");
            reconnect();
        }
        mqttClient.loop();
    }
}

void callback(char* topic, byte* message, unsigned int length) {

    String mqttMessage = "";
    String theTopic = String(topic);

    for (int i = 0; i < length; i++) {
        mqttMessage += (char)message[i];
    }

    Serial.println("topic: " + theTopic);
    Serial.println("message: " + mqttMessage);

    if (theTopic == mqttTopic) {
        digitalWrite(BUILTIN_LED, HIGH);
        digitalWrite(lockOpenPin, LOW);
        delay(20);
        digitalWrite(lockOpenPin, HIGH);
        delay(5000);
        digitalWrite(lockClosePin, LOW);
        delay(20);
        digitalWrite(lockClosePin, HIGH);
        digitalWrite(BUILTIN_LED, LOW);
    }
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(mqttDeviceName.c_str())) {
      Serial.println("connected");
      // Subscribe
      mqttClient.subscribe(mqttTopic.c_str());
      digitalWrite(BUILTIN_LED, LOW);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void wifiReconnect() {
    Serial.println("Connecting to " + wifiSsid);

    WiFi.begin(wifiSsid.c_str(), wifiPass.c_str());

    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < 30000)) {
        delay(500);
        Serial.print('.');
    }

    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
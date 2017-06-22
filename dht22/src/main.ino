#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <DHT.h>
#include <WiFiClientSecure.h>

#include "WifiWrapper.h"
#include "mqtt-settings.h"
#include "wifi-settings.h"

#define DEBUG 0
// #define DEBUG 1

WiFiClientSecure wifiClient;
Adafruit_MQTT_Client mqttClient(
    &wifiClient,
    MQTT_SERVER,
    MQTT_SERVER_PORT,
    MQTT_USERNAME,
    MQTT_PASSWORD
);

char buffer[10];

DHT dht(2, DHT22);

const char* fingerprint
    = "DA 11 50 D6 94 26 1A BE C4 10 30 C2 F6 0F 0E 75 46 07 F0 F5";

Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(
    &mqttClient, MQTT_TOPIC_PREFIX "/humidity"
);
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(
    &mqttClient, MQTT_TOPIC_PREFIX "/temperature"
);
Adafruit_MQTT_Publish heatindex = Adafruit_MQTT_Publish(
    &mqttClient, MQTT_TOPIC_PREFIX "/heatindex"
);

void setup() {
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, true);

#if DEBUG == 1
    Serial.begin(9600);
    Serial.setDebugOutput(true);
#endif

    // WifiWrapper::connect(WIFI_SSID, WIFI_USERNAME, WIFI_PASSWORD);
    WifiWrapper::connect(WIFI_SSID, WIFI_PASSWORD);

    dht.begin();

#if DEBUG == 1
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
#endif

    if (!wifiClient.connect(MQTT_SERVER, MQTT_SERVER_PORT)) {
        Serial.println("connection failed");
        return;
    }

    if (wifiClient.verify(fingerprint, MQTT_SERVER)) {
        Serial.println("certificate matches");
    } else {
        Serial.println("certificate doesn't match");
    }
}

void loop() {
    delay(2000);

    if (!mqttClient.connected()) {
        WifiWrapper::reconnect();

        while (mqttClient.connect() != 0) {
            // try again after five seconds
            delay(5000);
            digitalWrite(BUILTIN_LED, false);
            delay(50);
            digitalWrite(BUILTIN_LED, true);
            mqttClient.disconnect();
        }
    }

    // reading temperature or humidity takes about 250 milliseconds
    // sensor readings may also be up to 2 seconds 'old'
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();

    // check if any reads failed
    if (isnan(hum) || isnan(temp)) {
        // return will skip everything else in loop()
        return;
    }

    // compute heat index
    float hindex = dht.computeHeatIndex(temp, hum, false);

    dtostrf(hum, 4, 2, buffer);
    humidity.publish(buffer);
    flash_led();

    dtostrf(temp, 4, 2, buffer);
    temperature.publish(buffer);
    flash_led();

    dtostrf(hindex, 4, 2, buffer);
    heatindex.publish(buffer);
    flash_led();
}

void flash_led() {
    digitalWrite(BUILTIN_LED, false);
    delay(10);
    digitalWrite(BUILTIN_LED, true);
}

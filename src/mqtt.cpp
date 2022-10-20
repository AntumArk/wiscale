#include "mqtt.h"
#include "ArduinoJson.h"

WiFiClient espClient;
PubSubClient mqtt(espClient);


void setupMqtt()
{
    String clientId = WiFi.hostname();
    mqtt.setServer("broker.lan", 1883);
    mqtt.setCallback(configCallback);
    mqtt.setBufferSize(2048); // must set to larger, as by default it is limited to 256
    mqtt.connect(clientId.c_str());
    Serial.println("My ID");
    Serial.println(clientId);
    mqtt.subscribe("config/ESP-D4EFA3");//(char*)("config/"+clientId).c_str(), 1);
}

void loopMqtt() {
    mqtt.loop();
}

void destroyMqtt() {
    mqtt.disconnect();
}

void sendMessage()
{
    if (mqtt.connected()) {
        char msg[254];
        char topic[64];
        sprintf(msg
            , "{\"host\":\"%s\",\"grams\":%f,\"pieces\":%d,\"battery\":%f,\"configured\":%d,\"temperature\":%f,\"charging\":%d}"
            , config.name
            , state.grams
            , state.pieces
            , state.battery
            , state.configured
            , state.temperature
            , state.charging
        );
        sprintf(topic, "scale/%s/data", config.name);
        mqtt.publish(topic, msg, true);
    }
}

void configCallback(char* topic, byte* payload, unsigned int length) {
    Serial.println("Received from topic");
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    config.name = doc["name"];
    config.battery_range = doc["battery_range"];
    config.scale_zero = doc["scale_zero"];
    config.scale_cal = doc["scale_cal"];
    config.scale_gain = doc["scale_gain"];
    config.piece_grams = doc["piece_grams"];
    config.led_pin = doc["led"];
    state.configured = 1;
}


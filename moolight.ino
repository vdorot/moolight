#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <stdlib.h>
#include "const.h"

#include "LEDEngine.h"

LEDEngine engine;

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);


  mqttClient.subscribe("mode", 0);
  mqttClient.subscribe("color", 0);
  mqttClient.subscribe("brightness", 0);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}


void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {

  

  
  Serial.println("Message received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  payload: ");
  Serial.println(payload);

  if((String) topic == (String) "mode"){
      if((String) payload == (String) "color"){
        engine.setMode(LEDEngine::MODE::COLOR);
      } else if ((String) payload == (String ) "rainbow_beat"){
        engine.setMode(LEDEngine::MODE::RAINBOW);  
      }
  }

  if((String) topic == (String) "color"){
    if(strlen(payload) == 7){
      uint8_t r,g,b;
      long long int rgb = strtol(&payload[1], NULL, 16);
      r = ((rgb >> 16) & 0xFF);  // Extract the RR byte
      g = ((rgb >> 8) & 0xFF);   // Extract the GG byte
      b = ((rgb) & 0xFF); 
      
      CRGB color = CRGB(g, r, b);
      engine.setColor(color);  
      
    }
  }

  if((String) topic == (String) "brightness"){
    unsigned long int br = strtoul(payload, NULL, 10);
    engine.setBrightness(br);   
  }

}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();
  engine.setup();
}

void loop() {
  engine.update();
}

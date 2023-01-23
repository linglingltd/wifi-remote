#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "./credentials.h"

WiFiClient mqttClient;
PubSubClient mqtt(mqttClient);
#define MSG_SIZE 200
char mqttmsg[MSG_SIZE] = {'\0'};

// Initialisierung nach Batteriewechsel
const uint8_t initReq[] = {0x55, 0xAA, 0x01, 0x07, 0x00, 0x00, 0x07};
const uint8_t initResp[] = {0x55, 0xAA, 0x01, 0x07, 0x00, 0x48, 0x31, 0x2E, 0x30, 0x2E, 0x30, 0x00, 0xF9, 0x01, 0x77, 0x62, 0x33, 0x73, 0x00, 0x0A, 0x03, 0x14, 0x14, 0x14, 0x14, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x00, 0x00, 0x17, 0x02, 0x03, 0x04, 0x07, 0x1D, 0x06, 0x05, 0x14, 0x15, 0x16, 0x1E, 0x18, 0x19, 0x00, 0x00, 0x0D, 0x01, 0x02, 0x04, 0x07, 0x0F, 0x06, 0x05, 0x0A, 0x0B, 0x0C, 0x10, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x00, 0xC5};

// Group selection
const uint8_t grpA[] = {0x55, 0xAA, 0x01, 0x06, 0x00, 0x01, 0x00, 0x07};
const uint8_t grpB[] = {0x55, 0xAA, 0x01, 0x06, 0x00, 0x01, 0x01, 0x08};
const uint8_t grpResp[] = {0x55, 0xAA, 0x01, 0x06, 0x00, 0x01, 0x01, 0x08};

// Commands
const uint8_t cmdMoon[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x21, 0x02, 0x00, 0x00, 0x00, 0x2C};
const uint8_t cmdMonitor[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x21, 0x02, 0x02, 0x00, 0x00, 0x2E};
const uint8_t cmdBook[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x21, 0x02, 0x01, 0x00, 0x00, 0x2D};
const uint8_t cmdPalm[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x21, 0x02, 0x03, 0x00, 0x00, 0x2F};
const uint8_t cmdSliderLeftDown[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x25, 0x01, 0x00, 0x14, 0x00, 0x43};
const uint8_t cmdSliderLeftUp[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x25, 0x02, 0x00, 0x00, 0x00, 0x30};
const uint8_t cmdSliderRightDown[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x25, 0x00, 0x00, 0x14, 0x00, 0x42};
const uint8_t cmdSliderRightUp[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x25, 0x02, 0x00, 0x00, 0x00, 0x30};
const uint8_t cmdSquareLeftDown[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x27, 0x01, 0x00, 0x14, 0x00, 0x45};
const uint8_t cmdSquareLeftUp[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x27, 0x02, 0x00, 0x00, 0x00, 0x32};
const uint8_t cmdSquareRightDown[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x27, 0x00, 0x00, 0x14, 0x00, 0x44};
const uint8_t cmdSquareRightUp[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x27, 0x02, 0x00, 0x00, 0x00, 0x32};
const uint8_t cmdSquareTopDown[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x2A, 0x00, 0x14, 0x00, 0x00, 0x47};
const uint8_t cmdSquareTopUp[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x2A, 0x02, 0x00, 0x00, 0x00, 0x35};
const uint8_t cmdSquareBottomDown[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x2A, 0x01, 0x14, 0x00, 0x00, 0x48};
const uint8_t cmdSquareBottomUp[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x06, 0x01, 0x2A, 0x02, 0x00, 0x00, 0x00, 0x35};
const uint8_t cmdOn[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x03, 0xFF, 0x01, 0x01, 0x06};
const uint8_t cmdOff[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x03, 0xFF, 0x01, 0x00, 0x05};
const uint8_t cmdResp[] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x00, 0x02};

uint8_t msg[15] = {0}, msgptr = 0;
void setup() {
  pinMode(D4, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(9600);

  /*********************
     WiFi Connection
   *********************/
  // Set name passed to AP while connection
//  mdns::MDns myMdns();
  WiFi.hostname(NAME);
  // Connect to AP
  WiFi.begin(SSID, PWD);
  // Wait while not connected
  while (WiFi.status() != WL_CONNECTED) 
  {
    digitalWrite(D4, !digitalRead(D4));
    delay(100);
  }
  digitalWrite(D4, HIGH);
  // OTA setup in separate function
  setupOTA();
  msgptr = 0;

  /*************************
     MQTT Setup
   *************************/

  // Set IP and port of broker
  mqtt.setServer(MQTT_SERVER_IP, MQTT_PORT);
  // Set CB function to be called for all subscriptions
  mqtt.setCallback(mqttCallback);
  // Connect to broker using a unique ID (here the name)
  if (mqtt.connect(NAME)) {
    Serial.println("Connected to MQTT server");
    // Subscribe to a certain topic
    // mqtt.subscribe("sensor/Remote01");
  } else {
    mqtt.setKeepAlive(300);
    Serial.println("Cannot connect to MQTT server");
  }

  // Indicate end of setup
  Serial.println("Setup for MQTT done!");

  digitalWrite(D4, LOW);
}

void loop() {
  uint8_t i = 0;
  mqtt.loop();
  ArduinoOTA.handle();
  // put your main code here, to run repeatedly:
  if(Serial.available()) {
    if(msgptr == 0) {
      // Check for start
      msg[msgptr] = Serial.read();
      if(msg[msgptr] == 0x55) {
        msgptr++;
      }
    } else if(msgptr >= 15) {
      msgptr = 0;
      // Rotate shit.
    } else {
      msg[msgptr] = Serial.read();
      msgptr++;
    }

    if(msgptr == 7) {
      for(i=0;i<7;i++) {
        if(msg[i] != initReq[i]) i = 100;
      }
      if(i < 100) {
        // Got init sequence, answer with response
        Serial.write(initResp, sizeof(initResp));
        msgptr = 0;
      }
    }
    if(msgptr == sizeof(cmdOff)) {
      for(i=0;i<sizeof(cmdOff);i++) {
        if(msg[i] != cmdOff[i]) i = 100;
      }
      if(i < 100) {
        // Got init sequence, answer with response
        mqtt.publish("cmnd/kitchen/light1/POWER", "OFF", false); 
        mqtt.publish("cmnd/livingroom/light1/POWER", "OFF", false); 
        mqtt.publish("cmnd/bathroom/light1/POWER", "OFF", false); 
        Serial.write(cmdResp, sizeof(cmdResp));
        msgptr = 0;
      }
    }
    if(msgptr == sizeof(cmdOn)) {
      for(i=0;i<sizeof(cmdOn);i++) {
        if(msg[i] != cmdOn[i]) i = 100;
      }
      if(i < 100) {
        // Got init sequence, answer with response
        mqtt.publish("cmnd/kitchen/light1/POWER", "ON", false); 
        mqtt.publish("cmnd/livingroom/light1/POWER", "ON", false); 
        mqtt.publish("cmnd/bathroom/light1/POWER", "ON", false); 
        Serial.write(cmdResp, sizeof(cmdResp));
        msgptr = 0;
      }
    }
    if(msgptr == sizeof(cmdMoon)) {
      for(i=0;i<sizeof(cmdMoon);i++) {
        if(msg[i] != cmdMoon[i]) i = 100;
      }
      if(i < 100) {
        // Got init sequence, answer with response
        mqtt.publish("cmnd/kitchen/light1/POWER", "TOGGLE", false); 
        Serial.write(cmdResp, sizeof(cmdResp));
        msgptr = 0;
      }
    }
    if(msgptr == sizeof(cmdMonitor)) {
      for(i=0;i<sizeof(cmdMonitor);i++) {
        if(msg[i] != cmdMonitor[i]) i = 100;
      }
      if(i < 100) {
        // Got init sequence, answer with response
        mqtt.publish("cmnd/livingroom/light1/POWER", "TOGGLE", false); 
        Serial.write(cmdResp, sizeof(cmdResp));
        msgptr = 0;
      }
    }
    if(msgptr == sizeof(cmdPalm)) {
      for(i=0;i<sizeof(cmdPalm);i++) {
        if(msg[i] != cmdPalm[i]) i = 100;
      }
      if(i < 100) {
        // Got init sequence, answer with response
        mqtt.publish("cmnd/bathroom/light1/POWER", "TOGGLE", false); 
        Serial.write(cmdResp, sizeof(cmdResp));
        msgptr = 0;
      }
    }
    if(msgptr == sizeof(cmdSliderRightUp)) {
      for(i=0;i<sizeof(cmdSliderRightUp);i++) {
        if(msg[i] != cmdSliderRightUp[i]) i = 100;
      }
      if(i < 100) {
        // Got init sequence, answer with response
        mqtt.publish("cmnd/kitchen/light1/DIMMER", "100", false); 
        mqtt.publish("cmnd/bathroom/light1/DIMMER", "100", false); 
        mqtt.publish("cmnd/livingroom/light1/DIMMER", "100", false); 
        Serial.write(cmdResp, sizeof(cmdResp));
        msgptr = 0;
      }
    }
    if(msgptr == sizeof(cmdSliderLeftUp)) {
      for(i=0;i<sizeof(cmdSliderLeftUp);i++) {
        if(msg[i] != cmdSliderLeftUp[i]) i = 100;
      }
      if(i < 100) {
        // Got init sequence, answer with response
        mqtt.publish("cmnd/kitchen/light1/DIMMER", "5", false); 
        mqtt.publish("cmnd/bathroom/light1/DIMMER", "5", false); 
        mqtt.publish("cmnd/livingroom/light1/DIMMER", "5", false); 
        Serial.write(cmdResp, sizeof(cmdResp));
        msgptr = 0;
      }
    }
  }
  delay(25);
}

void setupOTA() {
  // Set name of this device (again)
  ArduinoOTA.setHostname(NAME);
  // Set a password to protect against uploads from other parties
  ArduinoOTA.setPassword(OTA_PWD);

  // CB function for when update starts
  ArduinoOTA.onStart([]() {
    Serial.println("Start updating");
  });
  // CB function for when update starts
  ArduinoOTA.onEnd([]() {
    Serial.println("Finished updating");
  });
  // CB function for progress
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Update at %u % \n", progress / (total / 100));
  });
  // CB function for when update was interrupted
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.println("Error updating");
    ESP.restart();
  });
  // Start OTA update service
  ArduinoOTA.begin();
}

void mqttCallback(char* topic, byte* message, unsigned int length) {
  // Convert message as byte array to cstring
  unsigned int len = min((int)length, (int)(MSG_SIZE - 1));
  memcpy(&mqttmsg[0], message, len);
  mqttmsg[len] = '\0';
  // Print over serial
  // Serial.printf("MQTT msg on topic: %s: %s\n", topic, &mqttmsg);
}

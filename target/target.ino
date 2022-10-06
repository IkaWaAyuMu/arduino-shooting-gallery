#define DECODE_NEC

#include <IRremote.hpp>
#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include "DEFINE.h"
// ------------------------------------------------------------------
using namespace websockets;
WebsocketsClient client;
// ------------------------------------------------------------------
void SetColor(int rValue, int gValue, int bValue);
// ------------------------------------------------------------------
bool isUp = false;
bool isBegin = false;

// ------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
    
  IrReceiver.begin(RECIEVER_PIN, ENABLE_LED_FEEDBACK);

  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  LEDSetColor(true, false, false);
  
  // Connections
  WiFi.begin(SSID, PSWD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  while (!client.connect(HOST, PORT, "/")) {
    Serial.print("+");
    delay(500);
  }
  Serial.println();
  Serial.println("CONNECTED");

  client.onMessage([&](WebsocketsMessage message) {
    Serial.print("Message recieved: ");
    String data = message.data();
    Serial.println(data);

    if (data == "TYPE") client.send("TARGET");

    else if (data == "UP") { isUp = true; }
    else if (data == "DOWN") { isUp = false; }
  });
  LEDSetColor(false,false,true);
  delay(500);  
}

void loop() {
  if(client.available()) {
    client.poll();
  }
  if (isUp) LEDSetColor(false,true,false);
  else LEDSetColor(false,false,false);

  if (IrReceiver.decode()) {
      IrReceiver.printIRResultShort(&Serial);
      if (isUp && IrReceiver.decodedIRData.protocol == NEC && IrReceiver.decodedIRData.command == 0x42) {
        Serial.println("Scored");
        client.send("SCORE");
        isUp = false;
        IrReceiver.resume();
      }
      else IrReceiver.resume();
  }
  delay(50);
}

void LEDSetColor(bool isROn,bool isGOn,bool isBOn) {
  digitalWrite(LED_R_PIN, !isROn);
  digitalWrite(LED_G_PIN, !isGOn);
  digitalWrite(LED_B_PIN, !isBOn);
}
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

  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  LEDSetColor(255,0,0);

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
  LEDSetColor(128,128,255);
  delay(500);

  client.onMessage([&](WebsocketsMessage message) {
    Serial.print("Message recieved: ");
    String data = message.data();
    Serial.println(data);    

    if (data == "TYPE") client.send("TARGET"); 

    else if (data == "UP") { isUp = true; }
    else if (data == "DOWN") { isUp = false; }
  });
}

void loop() {
  if(client.available()) {
    client.poll();
  }
  Serial.println(isBegin);
  if (isUp) {
    if (!isBegin) { IrReceiver.begin(RECIEVER_PIN, false); isBegin = true; LEDSetColor(0,255,0); }
    
    if (isBegin && IrReceiver.decode()) {
      IrReceiver.printIRResultShort(&Serial);
      if (IrReceiver.decodedIRData.protocol == NEC && IrReceiver.decodedIRData.address == 0x0690 && IrReceiver.decodedIRData.command == 0x42) {
        Serial.println("Scored");
        client.send("SCORE");
      }
      else IrReceiver.resume();
    }
  }
  else if (!isUp) {
    if (isBegin) { IrReceiver.stop(); LEDSetColor(0,0,0); isBegin = false;}
  }
  delay(50);
}

void LEDSetColor(int rValue, int gValue, int bValue) {
  analogWrite(LED_R_PIN, rValue);
  analogWrite(LED_G_PIN, gValue);
  analogWrite(LED_B_PIN, bValue);
}
#include <IRremote.hpp>
#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include "DEFINE.h"

// ------------------------------------------------------------------
using namespace websockets;
WebsocketsClient client;

Servo servo;
// ------------------------------------------------------------------
void UpdateFrame();
// ------------------------------------------------------------------
bool isUp = false;
bool isBegin = false;

// ------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  servo.attach(SERVO_PIN);

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
}

void loop() {
  if(client.available()) {
    client.poll();
  }
  Serial.println(isBegin);
  if (isUp) {
    if (!isBegin) { delay(200); IrReceiver.begin(RECIEVER_PIN, false); isBegin = true; }
    
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
    if (isBegin) { IrReceiver.stop(); isBegin = false;}
  }
  delay(50);
}
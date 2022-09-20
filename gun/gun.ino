#include <IRremote.hpp>
#include <AceTMI.h>
#include <AceSegment.h>
#include <ESP8266TimerInterrupt.h>

#include "DEFINE.h"

// ------------------------------------------------------------------
ESP8266Timer ITimer;
// ------------------------------------------------------------------
using ace_tmi::SimpleTmi1637Interface;
using TmiInterface = SimpleTmi1637Interface;
TmiInterface tmiInterface(TM1637_DIO, TM1637_CLK, TM1637_DELAY);
ace_segment::Tm1637Module<TmiInterface, TM1637_DIG> ledModule(tmiInterface);
// ------------------------------------------------------------------

// Function Prototypes
void UpdateFrame();
void FlushSegmentNumber(int count);
// ------------------------------------------------------------------

bool isForbiddenHold = false;  // Is weapon FIRE_BUTTON and RELOAD_BUTTON still holding
bool isFireHold = false;       // Is weapon FIRE_BUTTON still holding
bool isReloadHold = false;     // Is weapon RELOAD_BUTTON still holding
int ammo = AMMO_COUNT;         // Current ammo in magazine
int fireDelay = 0;             // Delay until the next fire available
int visualDelay = 0;           // Delay until the visual disappear
int reloadDelay = 0;           // Delay until the next magazine available

void setup() {
  Serial.begin(115200);

  if (ITimer.attachInterruptInterval(1000, UpdateFrame)) Serial.println("Interrupt initiated");

  tmiInterface.begin();
  ledModule.begin();
  ledModule.setBrightness(2);
  FlushDash();

  pinMode(FIRE_BUTTON, INPUT_PULLUP);
  pinMode(RELOAD_BUTTON, INPUT_PULLUP);
  pinMode(VISUAL_PIN, OUTPUT);

  IrSender.begin(IRLED, false);
}

void loop() {
  if (reloadDelay <= 0) FlushSegmentNumberNoZeroLead(ammo);
  else FlushSegmentNumberDashLead(reloadDelay);

  digitalWrite(VISUAL_PIN, visualDelay > 0);

  if (!digitalRead(FIRE_BUTTON) && !digitalRead(RELOAD_BUTTON)) isForbiddenHold = true;
  else if (digitalRead(FIRE_BUTTON) && digitalRead(RELOAD_BUTTON)) isForbiddenHold = false;

  if (reloadDelay <= 0 && !isForbiddenHold) {
    // FIRE
    if (!digitalRead(FIRE_BUTTON)) {
      if (((!ISAUTO && !isFireHold) || ISAUTO) && fireDelay <= 0 && ammo > 0) {
        Serial.println("FIRE");
        IrSender.sendNEC(0x0001, 0x01, 0);
        ammo--;
        fireDelay = FIRE_RATE;
        visualDelay = VISUAL_DELAY;
        isFireHold = true;
      }
    } else if (isFireHold) isFireHold = false;
    // RELOAD
    else if (!digitalRead(RELOAD_BUTTON)) {
      if (!isReloadHold) {
        Serial.println("RELOADING");
        reloadDelay = RELOAD_TIME;
        isReloadHold = true;
      }
    } else if (isReloadHold && reloadDelay <= 0) {
      ammo = AMMO_COUNT;
      isReloadHold = false;
    }
  }
}

// ------------------------------------------------------------------
// Functions

// Use interrupt to update time frame
void UpdateFrame() {
  if (fireDelay > 0) fireDelay--;
  if (visualDelay > 0) visualDelay--;
  if (reloadDelay > 0) reloadDelay--;
}
// Flush specified number to TM1637 LED
void FlushSegmentNumber(int count) {
  for (int i = 0; i < TM1637_DIG; i++) { ledModule.setPatternAt(i, NUM_DIGIT[(count % (int(pow(10, TM1637_DIG - i)))) / (int(pow(10, TM1637_DIG - i - 1)))]); }
  ledModule.flush();
}

void FlushSegmentNumberDashLead(int count) {
  int i = 0;
  for ( ; i < TM1637_DIG && count / int(pow(10,TM1637_DIG-i-1)) == 0 ; i++) { ledModule.setPatternAt(i, DASH); }
  for ( ; i < TM1637_DIG; i++) { ledModule.setPatternAt(i, NUM_DIGIT[(count % (int(pow(10, TM1637_DIG - i)))) / (int(pow(10, TM1637_DIG - i - 1)))]); }
  ledModule.flush();
}

void FlushSegmentNumberNoZeroLead(int count) {
  int i = 0;
  for ( ; i < TM1637_DIG-1 && count / int(pow(10,TM1637_DIG-i-1)) == 0 ; i++) { ledModule.setPatternAt(i, 0b00000000);}
  for ( ; i < TM1637_DIG; i++) { ledModule.setPatternAt(i, NUM_DIGIT[(count % (int(pow(10, TM1637_DIG - i)))) / (int(pow(10, TM1637_DIG - i - 1)))]);}
  ledModule.flush();
}

void FlushDash() {
  for (int i = 0; i < TM1637_DIG; i++) { ledModule.setPatternAt(i, DASH); }
  ledModule.flush();
}
// ------------------------------------------------------------------
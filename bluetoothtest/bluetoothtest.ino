#include "Arduino.h"

#define Bluetooth Serial3

void setup() {
  Bluetooth.begin(115200);
  Serial.begin(115200);
}

void loop() {
  Bluetooth.write(0xf5);
  Bluetooth.write(0x05);
  Bluetooth.write(0x07);
  Bluetooth.write(0x0A);
  Bluetooth.write(0);
  Bluetooth.write(0xE9);
  Bluetooth.flush();
  Serial.println("Send heartbeat");
  delay(1000);
}

#include <Arduino.h>
#include <Wire.h>
#include "Is7326.h"


#define IS31IO7326_I2C_ADDR_BASE 0x58


// Interrupt handlers.
// There can only be four I2C addresses for the controller,
// a 4-bit mask can encode the addresses that need to be read.
static volatile byte intrMask = 0;

#define keyDownFunc(num) static void keyDown##num() { intrMask |= (1 << num); }
keyDownFunc(0);
keyDownFunc(1);
keyDownFunc(2);
keyDownFunc(3);
#undef keyDownFunc

static void (*keyDowns[])() = {keyDown0, keyDown1, keyDown2, keyDown3};
static inline bool keyReady(byte ad01) { return (intrMask & (1 << ad01)) != 0; }
static inline void clearReady(byte ad01) { intrMask &= ~(1 << ad01); }


Is7326::Is7326(byte setAd01) {
  ad01 = setAd01 & 0x3;
  intrActive = 0;
}

Is7326::~Is7326() {}

void Is7326::attachInterrupt(byte interruptPin) {
  ::attachInterrupt(digitalPinToInterrupt(interruptPin), keyDowns[ad01], FALLING);
  intrActive = 1;
}

byte Is7326::setConfig(byte config) {
  Wire.beginTransmission(IS31IO7326_I2C_ADDR_BASE | ad01);
  Wire.write(0x8);
  Wire.write(config);
  return -Wire.endTransmission();
}

int Is7326::readConfig() {
  Wire.beginTransmission(IS31IO7326_I2C_ADDR_BASE | ad01);
  Wire.write(0x8);
  byte error = Wire.endTransmission();
  if (error != 0) {
    return -error;
  }

  byte bytes = Wire.requestFrom((IS31IO7326_I2C_ADDR_BASE | ad01), 1, true);
  if (bytes != 1) {
    while(Wire.available()) { Wire.read(); }
    return -(5 + bytes);
  }
  return Wire.read();
}

bool Is7326::isKeyReady() {
  return (!intrActive) ?: keyReady(ad01);
}

bool Is7326::readKey(key_t *key) {
  byte addr = IS31IO7326_I2C_ADDR_BASE | ad01;
  Wire.beginTransmission(addr);
  Wire.write(0x10);
  byte error = Wire.endTransmission();
  if (error != 0) {
    return false;
  }

  byte bytes = Wire.requestFrom(addr, 1, true);
  if (bytes != 1) {
    // Consume the input buffer, in case bytes > 1
    while(Wire.available()) { Wire.read(); }
    return false;
  }

  byte k = Wire.read();
  if ((k & 0x80) == 0) {
    // no extra keys, clear the interrupt flag for this address
    noInterrupts();
    clearReady(ad01);
    interrupts();
  }

  if (key) {
    key->down = (k & 0x40) != 0;
    key->key = k & 0x3f;
  }

  return intrActive ? true : (k & 0x3f) != 0;
}

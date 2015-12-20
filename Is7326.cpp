#include <Arduino.h>
#include <Wire.h>
#include "Is7326.h"

#define IS31IO7326_I2C_ADDR_BASE 0x58

// Interrupt handlers.
// There can only be four I2C addresses for the controller,
// a 4-bit mask can encode the addresses that need to be read.
static volatile uint8_t intrMask = 0;

#define keyDownFunc(num) static void keyDown##num() { intrMask |= (1 << num); }
keyDownFunc(0);
keyDownFunc(1);
keyDownFunc(2);
keyDownFunc(3);
#undef keyDownFunc

static void (*keyDowns[])() = {keyDown0, keyDown1, keyDown2, keyDown3};
static inline bool keyReady(uint8_t ad01) { return (intrMask & (1 << ad01)) != 0; }
static inline void clearReady(uint8_t ad01) { intrMask &= ~(1 << ad01); }


Is7326::Is7326(uint8_t setAd01) {
  ad01 = setAd01 & 0x3;
  intrActive = 0;
  key0State = 0;
}

Is7326::~Is7326() {}

void Is7326::attachInterrupt(uint8_t interruptPin) {
  ::attachInterrupt(digitalPinToInterrupt(interruptPin), keyDowns[ad01], FALLING);
  intrActive = 1;
}

int8_t Is7326::setConfig(uint8_t config) {
  Wire.beginTransmission(IS31IO7326_I2C_ADDR_BASE | ad01);
  Wire.write(0x8);
  Wire.write(config);
  return -Wire.endTransmission();
}

int8_t Is7326::readConfig(uint8_t *config) {
  Wire.beginTransmission(IS31IO7326_I2C_ADDR_BASE | ad01);
  Wire.write(0x8);
  int8_t error = Wire.endTransmission();
  if (error != 0) {
    return -error;
  }

  uint8_t bytes = Wire.requestFrom((IS31IO7326_I2C_ADDR_BASE | ad01), 1, true);
  if (bytes != 1) {
    while(Wire.available()) { Wire.read(); }
    return -(5 + bytes);
  }
  *config = Wire.read() & 0x7F;
  return 0;
}

bool Is7326::isKeyReady() {
  return keyReady(ad01);
}

int8_t Is7326::readKey(is7326_key_t *key) {
  uint8_t addr = IS31IO7326_I2C_ADDR_BASE | ad01;
  Wire.beginTransmission(addr);
  Wire.write(0x10);
  int8_t error = Wire.endTransmission();
  if (error != 0) {
    return -error;
  }

  uint8_t bytes = Wire.requestFrom(addr, 1, true);
  if (bytes != 1) {
    while(Wire.available()) { Wire.read(); }
    return -(5 + bytes);
  }

  *(uint8_t *)key = Wire.read();
  if (intrActive && !key.reserved) {
    // no extra keys, clear the interrupt flag for this address
    noInterrupts();
    clearReady(ad01);
    interrupts();
  }

  if (intrActive == 0 && key.code == 0) {
    if (key.down == key0State) {
      return UINT8_MIN;
    } else {
      key0State = key.down;
    }
  }

  return 0;
}

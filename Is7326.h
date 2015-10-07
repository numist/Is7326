#pragma once

#include <Arduino.h>


typedef struct {
  uint8_t down:1,
          key:6;
} key_t;


// config options
#define AUTO_CLEAR_INT_DISABLED 0x00
#define AUTO_CLEAR_INT_5MS      0x20
#define AUTO_CLEAR_INT_10MS     0x40

#define INPUT_PORT_FILTER_ENABLE  0x10
#define INPUT_PORT_FILTER_DISABLE 0x00

#define KEY_SCAN_DEBOUNCE_TIME_DOUBLE_6_8MS 0x00
#define KEY_SCAN_DEBOUNCE_TIME_NORMAL_3_4MS 0x08

#define LONGPRESS_DETECT_ENABLE  0x04
#define LONGPRESS_DETECT_DISABLE 0x00

#define LONGPRESS_DELAY_20MS 0x00
#define LONGPRESS_DELAY_40MS 0x01
#define LONGPRESS_DELAY_1S   0x02
#define LONGPRESS_DELAY_2S   0x03


class Is7326 {
public:
  // Initialize object with address ad01
  Is7326(byte ad01);
  ~Is7326();

  // attaches the proper interrupt controller to the given PIN
  // TODO: doc that not attaching interrupt means key 0 is not supported
  void attachInterrupt(byte interruptPin);

  // returns < 0 on error
  byte setConfig(byte config);

  // returns < 0 on error, otherwise returns the configuration register
  int readConfig();

  // returns true if a key is ready to be read
  bool isKeyReady();

  // gives information on the key that was just pressed or released.
  // you should call iskeyReady() first
  bool readKey(key_t *keyInfo);

private:
  byte ad01:2,
       intrActive:1;
};

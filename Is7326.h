#pragma once

#include <Arduino.h>

typedef struct {
    uint8_t code:6,
            down:1,
            reserved:1;
} is7326_key_t;

/*
 * Configuration options
 *
 * The options below can be ORed together to configure the IS31IO7326
 *****************************************************************************/

// Auto Clear INT: Interrupt pin timeout configuration
// IS31IO7326 default: DISABLED
#define IS7326_CONFIG_ACI_DISABLED 0x00
#define IS7326_CONFIG_ACI_5MS      0x20
#define IS7326_CONFIG_ACI_10MS     0x40

// Input Port Filter Enable: Filter port changes for 100ns before responding
// IS31IO7326 default: DISABLE
#define IS7326_CONFIG_DE_DISABLE 0x00
#define IS7326_CONFIG_DE_ENABLE  0x10

// Key Scan Debounce Time: Key scan interval for debouncing
// IS31IO7326 default: NORMAL_3_4MS
#define IS7326_CONFIG_SD_DOUBLE_6_8MS 0x00
#define IS7326_CONFIG_SD_NORMAL_3_4MS 0x08

// Long-pressed Key Detect Enable: periodic reporting of long-pressed keys
// IS31IO7326 default: DISABLE
#define IS7326_CONFIG_LE_DISABLE 0x00
#define IS7326_CONFIG_LE_ENABLE  0x04

// Long-pressed Key Detect Delay Time: Reporting interval for long-pressed keys
// IS31IO7326 default: 20MS
#define IS7326_CONFIG_LT_20MS 0x00
#define IS7326_CONFIG_LT_40MS 0x01
#define IS7326_CONFIG_LT_1S   0x02
#define IS7326_CONFIG_LT_2S   0x03


class Is7326 {
public:
  /*
   * Initialize Wire connection object with lowest two address bits ad01
   ***************************************************************************/
  Is7326(uint8_t ad01);

  ~Is7326();

  /*
   * Enables isKeyReady reporting for this client.
   *
   * Parameter interruptPin: INT pin of the IS31IO7326.
   *
   * NOTE: The IS31IO7326 supports polling for new values, but returns 0x00
   * when there is no new data to report. This means polling code cannot trust
   * key events for key 0.
   ***************************************************************************/
  void attachInterrupt(uint8_t interruptPin);

  /*
   * Sets the IS31IO7326's configuration.
   *
   * Valid configuration values are defined and described above with the prefix
   * IS7326_CONFIG, and should be ORed together. The top bit is reserved and
   * should not be used.
   *
   * Returns 0 on success, < 0 on error.
   ***************************************************************************/
  int8_t setConfig(uint8_t config);

  /*
   * Reads the configuration register from the IS31IO7326 into the parameter's
   * memory.
   *
   * Returns 0 on success, < 0 on error.
   ***************************************************************************/
  int8_t readConfig(uint8_t *config);

  /*
   * Returns true if a key is ready to be read.
   *
   * NOTE: This function always returns false if attachInterrupt has not been
   * called.
   ***************************************************************************/
  bool isKeyReady();

  /*
   * Reads key event information from the IS31IO7326 into the parameter's
   * memory.
   *
   * Returns 0 on success, < 0 on error.
   *
   * NOTES: This function may return key 0 event information when there is no
   * new data to report, which means applications that have not attached an
   * interrupt cannot trust key event data for key 0.
   *
   * If the scanner is using the is31io7326_clone firmware, this function will
   * correctly return an error code when there is no new key event information
   * available, even in the absence of an attached interrupt. Despite this
   * behaviour, isKeyReady will still return false.
   ***************************************************************************/
  int8_t readKey(is7326_key_t *key);

private:
  uint8_t ad01:2,
          intrActive:1,
          key0State:1;
};

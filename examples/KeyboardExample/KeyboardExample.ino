#include <Wire.h>
#include <Is7326.h>

Is7326 controller(0);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  controller.setConfig(IS7326_CONFIG_SD_NORMAL_3_4MS);
  controller.attachInterrupt(2);
}

void loop() {
  if (controller.isKeyReady()) {
    int8_t k = controller.readKey();
    if (k >= 0) {
      int8_t configr = controller.readConfig();
      if (configr >= 0) {
        Serial.print("\nConfiguration: ");
        Serial.print(configr, BIN);
        Serial.print("\n");
      }

      Serial.print("Controller ");
      if (k & IS7326_KEY_DOWN_MASK) {
        Serial.print(" pressed key ");
      } else {
        Serial.print("released key ");
      }
      Serial.print(k & IS7326_KEY_CODE_MASK, DEC);
      Serial.print("\n");
    }
  }

  delay(100);
}

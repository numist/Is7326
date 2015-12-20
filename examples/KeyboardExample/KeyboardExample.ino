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
    is7326_key_t key;
    if (!controller.readKey(&key)) {
      int8_t configr;
      if (!controller.readConfig(&configr)) {
        Serial.print("\nConfiguration: ");
        Serial.print(configr, BIN);
        Serial.print("\n");
      }

      Serial.print("Controller ");
      if (key.down) {
        Serial.print(" pressed key ");
      } else {
        Serial.print("released key ");
      }
      Serial.print(key.code, DEC);
      Serial.print("\n");
    }
  }

  delay(100);
}

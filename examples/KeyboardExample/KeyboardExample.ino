#include <Wire.h>
#include <Is7326.h>

Is7326 controller(0);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  controller.setConfig(INPUT_PORT_FILTER_ENABLE |
                      KEY_SCAN_DEBOUNCE_TIME_NORMAL_3_4MS);
  controller.attachInterrupt(2);
}

void loop() {
  key_t k;
  if (controller.isKeyReady() && controller.readKey(&k)) {
    Serial.print("\nConfiguration: ");
    Serial.print(controller.readConfig(), BIN);
    Serial.print("\n");

    Serial.print("Controller ");
    if (k.down) {
      Serial.print(" pressed key ");
    } else {
      Serial.print("released key ");
    }
    Serial.print(k.key, DEC);
    Serial.print("\n");
  }

  delay(1);
}

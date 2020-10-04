#include <Adafruit_RGBLCDShield.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

int incomingByte;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
}

void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if (incomingByte == 'E') {
      estop();
    }
    if (incomingByte == 'A') {
      arm();
    }
    if (incomingByte == 'F') {
      fire();
    }
  }
}

void estop() {
  lcd.clear();
  lcd.print("ESTOP");
  Serial.println("ESTOP ACKNOWLEDGED!");
  delay(1000);
}

void arm() {
  lcd.clear();
  lcd.print("ARM  ");
  Serial.println("ARM ACKNOWLEDGED!");
  delay(1000);
}

void fire() {
  lcd.clear();
  lcd.print("FIRE ");
  Serial.println("FIRE ACKNOWLEDGED!");
  delay(1000);
}

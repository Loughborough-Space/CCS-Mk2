#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <elapsedMillis.h>

const int p_igniterRelay = 15;
const int p_igniterDetector = 16;


elapsedMillis timeElapsed;

void setup() {
  pinMode(p_igniterRelay, OUTPUT);
  pinMode(p_igniterDetector, INPUT);
}


void loop() {
  // put your main code here, to run repeatedly:

}


bool igniterContinuityTest() {
  Serial.println("STARTING IGNITER CONTINUITY TEST");
  digitalWrite(p_igniterRelay, LOW);
  int igniterTestDuration = 5;
}


// In use: Only called if `if (rf95.available()) {}` statement passes, so is guaranteed to have a message

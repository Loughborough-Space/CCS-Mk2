// Chimera Control System - LAUNCHPAD (v2.0)
// Created by : Theo Ajuyah, Jack Kent, Sharar Khondker, et al.

// RF Code Courtesy: https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts/rfm9x-test


// Libraries
#include <SPI.h>
#include <RH_RF95.h>
#include "Adafruit_MAX31855.h"
#include <max6675.h>


// Stepper Motor Pins:
const int stepPin = 16;   //pin to pulse for steps
const int dirPin = 17;    //pin to change step direction
#define StartPosition 18 //the current code does not use interrupts, butten is high when steppermotor is in starting position

// Igniter Pin:
#define igniter 15

// Thermocouple A Pins:
int thermoA_DO = 4;
int thermoA_CS = 5;
int thermoA_CLK = 6;
MAX6675 thermocoupleA(thermoA_CLK, thermoA_CS, thermoA_DO);

// Thermocouple B Pins:
int thermoB_DO = 7;
int thermoB_CS = 8;
int thermoB_CLK = 9;
MAX6675 thermocoupleB(thermoB_CLK, thermoB_CS, thermoB_DO);

// Pressure Trandsducer A Pins:

// Pressure Trandsducer B Pins:

// LoRa Variables
#define MAXDO 8
#define MAXCS 7
#define MAXCLK 6
#define ID "C-" // LoRa radio ID (must match Base Station ID)
int16_t packetnum = 0;
unsigned short interrupt = 0;
#define RFM95_CS 5 // LoRa CS Pin
#define RFM95_RST 4 // LoRa RST Pin
#define RFM95_INT 3 // LoRa C0 Pin (must be hardware interrupt capable pin)
#define RF95_FREQ 868.0 // Desired radio frequency
RH_RF95 lora(RFM95_CS, RFM95_INT); // Initialises LoRa class
#define RH_RF95_MAX_MESSAGE_LEN 300 // Max radio message length

// Command Codes
#define ARM "ARM"
#define FIRE "FIRE"
#define ESTOP "ESTOP"

// Command Statuses
bool acking = false;
bool igniterrun = false;
bool EmergencyStop = false;

// Test sequence variables (need revising)
enum State {Idle, PreBurnWait, MainBurnWait} state;
#define durationMillis_preBurn (10 * 1000)
#define durationMillis_mainBurn (17 * 1000)
unsigned long startTimeMillis;


void setup() {
  // Pin Setup
  pinMode(igniter, OUTPUT);
  digitalWrite(igniter, HIGH);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(StartPosition, INPUT);
  pinMode(RFM95_RST, OUTPUT);

  // Begin serial
  Serial.begin(9600);
  Serial.println("Hello World!");

  // Initialise LoRa
  init_transceiver();

  // Reset stepper motor position
  Serial.println("Resetting stepper motor position...");
  go_to_start();
}


void loop()
{
  Serial.println(readSensors()); // Print sensor data
  String msg = ""; // Creates empty string to hold message

  if (lora.available()) {
    msg = getMessage(); // Get incoming message

    if (msg.substring(0, sizeof(ID) - 1) == ID) { // If starts with id e.g. .substring(0,2) == "C-" checks if 1st 2 letters match
      if (msg.substring(sizeof(ID) - 1, 2 + sizeof(ARM) - 1) == ARM) { // If command is "ARM"
        arm();
      } else if (msg.substring(sizeof(ID) - 1, 2 + sizeof(FIRE) - 1) == FIRE) { // If command is "FIRE"
        fire();
      } else if (msg.substring(sizeof(ID) - 1, 2 + sizeof(ESTOP) - 1) == ESTOP) {  // If command is "ESTOP"
        estop();
        EmergencyStop = true; // Permanently sets the estop status, to prevent later commands from overriding the estop
      } else {
        Serial.println("Command not recognised");
        Serial.println("Got this instead: " + msg);
      }
    } else {
      Serial.println("\""  ID  "\" check failed");
      Serial.println("Got this instead: " + msg);
    }

    String sensorData = readSensors(); // Gets sensor data
    sendMessage("C-", sensorData); // Send sensor data to base station
  }
}



// __________________________________________________| = Command Functions = |__________________________________________________

// Arm procedure
void arm() {
  Serial.println("Arm command received!");
}

// Fire procedure
void fire() {
  digitalWrite(igniter, LOW); // TODO: Check if this should be LOW or HIGH

  // Rest of the function below this neeeds doing, just collated code from accross old functions
  
  Serial.println("Entered igniter fun");
  turn_right_degrees(45);
  delay(4000);
  turn_right_degrees(45);
  Serial.println("About to delay!");

  delay(3000);
  Serial.println("Done delay");
  turn_right_degrees(-80);  //force close in case of an error
  go_to_start();
  Serial.println("fun flames");


  delay(2000);
  Serial.println("Left Igniter Fun");

  void fire() {

    if (igniterrun == false) {
              //igniterFun();
              igniterFun_part1();
              startTimeMillis = millis(); // set start time for 4s wait reference
              state = PreBurnWait;
              Serial.println("     About to go to PreBurnWait");
            }
            igniterrun = true;
    turn_right_degrees(45);
    delay(4000);
    turn_right_degrees(45);
    delay(17000);
    turn_right_degrees(-80);  //force close in case of an error
    go_to_start();
  }
}

// Emergency Stop
void estop() {
  Serial.println("!! EMERGENCY STOP !!");
  digitalWrite(igniter, HIGH);
  go_to_start();
}



// __________________________________________________| = LoRa Functions = |__________________________________________________

// Initialise LoRa transceiver module
void init_transceiver() {
  digitalWrite(RFM95_RST, HIGH);
  delay(100);

  Serial.println("Initiliasing LoRa...");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!lora.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa initialised!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!lora.setFrequency(RF95_FREQ)) {
    Serial.println("LoRa setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  lora.setTxPower(23, false);
}

// Gets the radio message: Only call if `if (lora.available()) {}` statement passes, so it's guaranteed to have a message
String getMessage() {
  // Should be a message for us now
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (lora.recv(buf, &len))
  {
    //RH_RF95::printBuffer("Received: ", buf, len); // suppresed/commented out printing binary info
    //Serial.println("Got: " + (char*)buf); // suppresed/commented out message received
    // Serial.print("RSSI: "); Serial.println(lora.lastRssi(), DEC); // suppresed/commented out Received Signal Strength Indicator

    return (char*)buf;

    // Send a reply
    uint8_t data[] = "Message received";
    lora.send(data, sizeof(data));
    lora.waitPacketSent();
    Serial.println("Sent a reply");
  } else {
    Serial.println("Receive failed");
    return "-1";
  }
}

// Send a radio message
void sendMessage(String receiverID, String msg) {
    //void sendMessage() {
    Serial.println("Sending to rf95_server");
    String message = receiverID + msg;
    char radiopacket[50];
    strncpy(radiopacket, message.c_str(), 50);//**** 3rd url

    itoa(packetnum++, radiopacket + 13, 10);
    Serial.print("Sending "); Serial.println(radiopacket);
    radiopacket[49] = 0;


    Serial.println("Sending..."); delay(10);
    lora.send((uint8_t *)radiopacket, 50);

    Serial.println("Waiting for packet to complete..."); delay(10);
    lora.waitPacketSent();
    // Now wait for a reply
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    Serial.println("Waiting for reply..."); delay(10);
    if (lora.waitAvailableTimeout(1000))
    {
      // Should be a reply message for us now
      if (lora.recv(buf, &len))
      {
        Serial.print("Got reply: ");
        Serial.println((char*)buf);
        Serial.print("RSSI: ");
        Serial.println(lora.lastRssi(), DEC);
      }
      else
      {
        Serial.println("Receive failed");
      }
    }
    else
    {
      Serial.println("No reply, is there a listener around?");
    }
}



// __________________________________________________| = Stepper Motor Functions = |__________________________________________________

// Go left to starting position
void go_to_start() {
  for (int i = 0; (i < 360) & (digitalRead(StartPosition) == 0); i++) turn_right_degrees(-1); //go till button closes or a full rotation if button fails
}

// Change the stepper direction to forward
void stepperFWD() {
  digitalWrite(dirPin, HIGH);
}

// Change the stepper direction to reverse
void stepperREV() {
  digitalWrite(dirPin, LOW);

}

// Have the stepper motor take one step
void motorStep() {
  digitalWrite(stepPin, HIGH);
  delay(30); // Don't go below 15 for the big one! With the
  digitalWrite(stepPin, LOW);
}

// Turn right by approximately the angle specified
// Note: Unlike servo motor, can't tell if moved by external force!
float turn_right_degrees(float angle) {
  if (angle > 0) // if +ve angle
    stepperFWD();
  else {
    stepperREV();
    angle *= -1; // make +ve for rest of calculation
  }

  int steps = angle / 1.8; // truncated # of steps
  float err = angle - steps * 1.8; // error(+ve) for under-shot
  if (err > 1.8 / 2) {
    steps++;
    err = err - 1.8; // error(-ve) for over-shot
  }
  err *= -1; // negates err so +ve means over-shot & vice versa
  err = (err == 0) ? 0 : err; // Just makes it display 0 & not -0

  for (int i = 0; i < steps; i++) {

    motorStep();
    //delay(1);
  }

  return err;
}

void turn_right_degrees_90() {
  turn_right_degrees(90);
  Serial.println("Turning right...");
}



// __________________________________________________| = Sensor Functions = |__________________________________________________

// Returns data from all sensors
String readSensors() {
  String sensorData;
  String tempData = readTemps();
  String pressData = readPress();
  sensorData += tempData;
  sensorData += pressData;
  return sensorData;
}


// Returns temperature data from both thermocouples
String readTemps() {
  //float tempA = thermocoupleA.readCelsius();
  //float tempB = thermocoupleB.readCelsius();
  float tempA = 25.2;
  float tempB = 27.8;
  String tbuf;
  tbuf += F("[Temps A:");
  tbuf += String(tempA, 6);
  tbuf += F(" B:");
  tbuf += String(tempB, 6);
  return tbuf;
}

// Returns pressure data from both pressure transducers
String readPress() { // Steal Sharar's code and put it in here
  String pressDat = String("[Press: A:100 B:100]");
  return pressDat;
}

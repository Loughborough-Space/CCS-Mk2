#include <SPI.h>
#include <RH_RF95.h>
#include "Adafruit_MAX31855.h"
#include <elapsedMillis.h>

#define RFM95_CS 5
#define RFM95_RST 4 
#define RFM95_INT 3
#define RF95_FREQ 868.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);
#define RH_RF95_MAX_MESSAGE_LEN 300


const int p_igniterRelay 15
const int p_igniterDetector 15
#define MAXDO   8
#define MAXCS   7
#define MAXCLK  6
#define ID "C-"

//Global Variables
int16_t packetnum = 0;
unsigned short interrupt = 0;

// Command codes
#define ARM "ARM"
#define FIRE "FIRE"
#define ESTOP "ESTOP"

bool acking = false;
bool igniterrun = false;
bool EmergencyStop = false;

enum State{Idle, PreBurnWait, MainBurnWait}state;
#define durationMillis_preBurn (10 * 1000)
#define durationMillis_mainBurn (17 * 1000)
unsigned long startTimeMillis;

Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);


elapsedMillis timeElapsed;

void setup() {
  pinMode(p_igniterRelay, OUTPUT)
  pinMode(P_igniterDetector, INPUT)
}


void loop() {
  // put your main code here, to run repeatedly:

}


bool igniterContinuityTest() {
  Serial.println("STARTING IGNITER CONTINUITY TEST");
  digitalWrite(p_igniterRelay, LOW);
  int igniterTestDuration = 
}


void initTransceiver() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  delay(100);
 
  Serial.println("Tranceiver Test!");
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}


// In use: Only called if `if (rf95.available()) {}` statement passes, so is guaranteed to have a message
String getMessage() {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      //RH_RF95::printBuffer("Received: ", buf, len); // suppresed/commented out printing binary info
      //Serial.println("Got: " + (char*)buf); // suppresed/commented out message received
      // Serial.print("RSSI: "); Serial.println(rf95.lastRssi(), DEC); // suppresed/commented out Received Signal Strength Indicator

      return (char*)buf;
      
      // Send a reply
      uint8_t data[] = "Message received";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
    }else {
      Serial.println("Receive failed");
      return "-1";
    }
}


void sendMessage(String receiverID, String msg) {
//void sendMessage() {
  Serial.println("Sending to rf95_server");
  String message = receiverID + msg;
  char radiopacket[50];
  strncpy(radiopacket, message.c_str(), 50);//**** 3rd url
  
  itoa(packetnum++, radiopacket+13, 10);
  Serial.print("Sending "); Serial.println(radiopacket);
  radiopacket[49] = 0;
  
  
  Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *)radiopacket, 50);
 
  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
 
  Serial.println("Waiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
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

void efun(){
  Serial.println("Emergency stop")
  digitalWrite(igniter, HIGH)
  go_to_start()
}

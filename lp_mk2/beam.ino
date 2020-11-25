#include <RH_RF95.h>

#define RFM95_CS 5 // Arduino PIN connected to LoRa Module 'CS'
#define RFM95_RST 4 // Arduino PIN connected to LoRa Module 'RST'
#define RFM95_INT 3
#define RF95_FREQ 868.0 // Frequency (MHz) of LoRa Module
RH_RF95 rf95(RFM95_CS, RFM95_INT);
#define RH_RF95_MAX_MESSAGE_LEN 300
#define MAXDO   8
#define MAXCS   7
#define MAXCLK  6
#define ID "LP" // Unique identifier of the packet sender

int16_t packetnum = 0;
unsigned short interrupt = 0;

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


String getMessage() {
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
  
    if (rf95.recv(buf, &len))
    {
      if(msg.substring(0,sizeof(ID)-1) == ID) {
        return (char*)buf;
      } else {
        Serial.println("! ID Check Failed.  Got: "+ msg.substring(0,sizeof(ID)-1));
      }
      
      // Send a reply
      uint8_t data[] = "Message received";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
    } else {
      Serial.println("Receive failed");
      return "-1";
    }
  }
}

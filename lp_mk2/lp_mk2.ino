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
  String msg = "";
  
    msg = getMessage();
    
      if(msg.substring(sizeof(ID)-1,2 + sizeof(ARM)-1) == ARM) {
        // Do this
        solenoidFun();
        //sendMessage("C-", "ARMACK");
        acking = true;
        //
      } else {
        Serial.println("\""  ARM  "\" check failed");
        Serial.println("Got this instead: " + msg);
        if(msg.substring(sizeof(ID)-1,2 + sizeof(FIRE)-1) == FIRE) {
          Serial.println("\""  FIRE  "\" was sent to \""  ID  "\"(that's me!)");
          
          if(igniterrun == false){
            //igniterFun();
            igniterFun_part1();
            startTimeMillis = millis(); // set start time for 4s wait reference
            state = PreBurnWait;
            Serial.println("     About to go to PreBurnWait");
          }
          igniterrun = true;
        } else {
          Serial.println("\""  FIRE  "\" check failed");
          Serial.println("Got this instead: " + msg);
        } 
         if(msg.substring(sizeof(ID)-1,2 + sizeof(ESTOP)-1) == ESTOP) {
          Serial.println("\""  ESTOP  "\" was sent to \""  ID  "\"(that's me!)");
          //eFun(); // called when a wait state is broken out of below based on EmergencyStop flag
          EmergencyStop = true;
        } else {
          Serial.println("\""  ESTOP  "\" check failed");
          Serial.println("Got this instead: " + msg);
        }
      }
    } else {
      Serial.println("\""  ID  "\" check failed");
      Serial.println("Got this instead: " + msg);
    }

//    if (acking) {
//      //sendMessage("C-", "ARMACK");
//    }else {
//      sendMessage("C-", "T-" + String(thermocouple.readCelsius()));
//    }
    sendMessage("C-", "T-" + String(thermocouple.readCelsius()));
  }

  switch(state) {
    case Idle:
      // statements
      //delay(2000);
      break;

    case PreBurnWait:
      // statements
      if (millis() - startTimeMillis < durationMillis_preBurn) { // if we haven't elapsed the desired duration...
        // if e-stop... do what u need to do, then big delay to 'stop' (more elegant solution later)
        if(EmergencyStop) { //{delay(60 *60 * 1000);} // 1hr
          eFun();
          state = Idle;
          delay(60 *60 * 1000); // leave this if e-stop is still being sent continuously
        }
      }else {
        // change state
        igniterFun_part2();
        startTimeMillis = millis(); // set start time for 17s wait reference
        state = MainBurnWait;
        Serial.println("     About to leave PreBurnWait to go to MainBurnWait");
      }
      break;

    case MainBurnWait:
      // statements
      if (millis() - startTimeMillis < durationMillis_mainBurn) { // if we haven't elapsed the desired duration...
        // if e-stop... do what u need to do, then big delay to 'stop' (more elegant solution later)
        if(EmergencyStop) {// {delay(60 *60 * 1000);} // 1hr
          eFun();
          state = Idle;
          delay(60 *60 * 1000); // leave this if e-stop is still being sent continuously
        }
      }else {
        // change state
        igniterFun_part3(); // no more waits after this
        state = Idle;
        Serial.println("     About to leave MainBurnWait");
      }
      break;

    default:
      // defult statements
      ;
  }
}


bool igniterContinuityTest() {
  Serial.println("STARTING IGNITER CONTINUITY TEST");
  digitalWrite(p_igniterRelay, LOW);
  int igniterTestDuration = 5;
}


// In use: Only called if `if (rf95.available()) {}` statement passes, so is guaranteed to have a message

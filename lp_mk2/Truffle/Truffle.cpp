/*
  Tuffle.h - Library for working with the LboroSpace Truffle Protocol.
  Created by Jack Kent, October 2020.
*/

#include "Arduino.h"
#include "Truffle.h"

Truffle::Truffle(String )
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void Truffle::dot()
{
  digitalWrite(_pin, HIGH);
  delay(250);
  digitalWrite(_pin, LOW);
  delay(250);  
}

void Truffle::dash()
{
  digitalWrite(_pin, HIGH);
  delay(1000);
  digitalWrite(_pin, LOW);
  delay(250);
}


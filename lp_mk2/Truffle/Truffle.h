/*
  Tuffle.h - Library for working with the LboroSpace Truffle Protocol.
  Created by Jack Kent, October 2020.
*/

#ifndef Truffle_h
#define Truffle_h

#include "Arduino.h"

class Truffle
{
  public:
    Truffle(int pin);
    void dot();
    void dash();
  private:
    int _pin;
};

#endif


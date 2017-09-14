#ifndef LineSensor3_H
#define LineSensor3_H
#include "Arduino.h"
  
  class DeviceI2C
 {
  public:
    DeviceI2C(int address,byte id,char type);
    byte GetID();
    int GetAddress();
    char GetType();
  private:
  	int address;
  	byte id;
  	char type;
 };

 #endif

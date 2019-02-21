#ifndef MODULE_H
#define MODULE_H
#include "Arduino.h"
  
  class Module
 {
  public:
    Module(byte address,byte id,byte type);
    byte GetID();
    byte GetAddress();
    byte GetType();
  private:
  	byte address;      // Adress of CS
  	byte id;           // Identifyier for multiple modules of the same type
  	byte type;         // Type of the module which is send to the app.
 };

 #endif

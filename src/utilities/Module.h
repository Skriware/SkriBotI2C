#ifndef MODULE_H
#define MODULE_H
#include "Arduino.h"
#define CONNECTION_TYPE_SPI 0
#define CONNECTION_TYPE_I2C 1
#define CONNECTION_TYPE_SPI_I2C 2     //TO DO? OR not...
  class Module
 {
  public:
    Module(byte _address,byte id,byte type,byte _connection_type = CONNECTION_TYPE_SPI);
    byte GetID();
    byte GetAddress();
    byte GetType();
    byte GetConnectionType();
  private:
  	byte SPI_address;      // Adress of CS
  	byte I2C_address;
    byte id;           // Identifyier for multiple modules of the same type
  	byte type;         // Type of the module which is send to the app.
    byte connection_type;
 };

 #endif

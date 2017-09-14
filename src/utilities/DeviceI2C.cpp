#include "DeviceI2C.h"

DeviceI2C::DeviceI2C(int _address, byte _id, char _type){
address = _address;
id = _id;   
type = _type;
}

byte DeviceI2C::GetID(){
  return(id);
}

int DeviceI2C::GetAddress(){
  return(address);
}

char DeviceI2C::GetType(){
	return(type);
}


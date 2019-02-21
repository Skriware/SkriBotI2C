#include "Module.h"

Module::Module(byte _address, byte _id, byte _type){
address = _address;
id = _id;   
type = _type;
}

byte Module::GetID(){
  return(id);
}

byte Module::GetAddress(){
  return(address);
}

byte Module::GetType(){
	return(type);
}


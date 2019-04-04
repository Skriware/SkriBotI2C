#include "Module.h"

Module::Module(byte _address, byte _id, byte _type,byte _connection_type){
if(connection_type == CONNECTION_TYPE_SPI){
SPI_address = _address;
}else if(connection_type == CONNECTION_TYPE_I2C){
I2C_address = _address;
}
id = _id;   
type = _type;
connection_type=_connection_type;
}
byte Module::GetID(){
  return(id);
}
byte Module::GetAddress(){
	byte tmp;
	if(connection_type == CONNECTION_TYPE_SPI){
		tmp = SPI_address;
	}else if(connection_type == CONNECTION_TYPE_I2C){
		tmp = I2C_address;
	}
	return(tmp);
}
byte Module::GetType(){
	return(type);
}

byte Module::GetConnectionType(){
	return(connection_type);
}


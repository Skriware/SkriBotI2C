#include "SkriBotI2C.h"
 
  SkriBotI2C::SkriBotI2C(){
   Wire.begin();
   N_Rotors   = 0;
   N_DistSensors  = 0;
   N_LED          = 0;
   N_LineSensors  = 0;
   N_Scopes       = 0;
   nDevices = 0;
  
  }

  void SkriBotI2C::ScanForDevices(){
    byte error;
     for(byte tmpaddress  = 1; tmpaddress < 127; tmpaddress++ ){
        Wire.beginTransmission(tmpaddress);
        error = Wire.endTransmission();
        delay(100);
  
      if (error == 0){

        Serial.print("I2C device found at address 0x");

        if (tmpaddress<16)
        Serial.print("0");

            Serial.print(tmpaddress,HEX);
            Serial.println("  !");
            address[nDevices] = tmpaddress;
            nDevices++;

      }else if (error==4){
            Serial.print("Unknown error at address 0x");
            if (tmpaddress<16)
              Serial.print("0");
            Serial.println(tmpaddress,HEX);
      }    
            Serial.print(tmpaddress);
            Serial.println(" done");
    }
  if (nDevices == 0)
          Serial.println("No I2C devices found\n");
  else
          Serial.println("All done\n");     
  }


  void SkriBotI2C::IdentifyDevices(){

    char buff;
   for(int i = 0; i < nDevices ; i++){
     Wire.beginTransmission(address[i]);
     Wire.write('I');
     Wire.endTransmission();
      delay(100);
      Wire.requestFrom(address[i],1);
      int zz = 0;
      while(Wire.available()) { 
      buff = Wire.read();        
      }  
      delay(100); 

      switch(buff){
        case 'R':
            AddDCRotor(address[i],N_Rotors);
            Serial.print("LED ");
            Serial.print(N_Rotors);
            Serial.println(" added on address:");
            Serial.println(address[i]);
            N_Rotors++;
        break;
        case 'D':
            AddDistSensor(address[i],N_DistSensors);
            N_DistSensors++;
        break;
        case 'L':
            AddLineSensor(address[i],N_LineSensors);
          Serial.print("Line Sensor ");
            Serial.print(N_LineSensors);
            Serial.println(" added on address:");
            Serial.println(address[i]);
            N_LineSensors++;
        break;
        default:

        break;
      }       


   }



  }
  void SkriBotI2C::AddDCRotor(int _address, byte id){
    DeviceI2C *dcrotor = new DeviceI2C(_address,id,'R');
    DCRotors[id] = dcrotor;
     Wire.beginTransmission(_address);
     Wire.write(id);
     Wire.endTransmission();
       
  }

  void SkriBotI2C::AddDistSensor(int _address, byte id){
    DeviceI2C *dsensor = new DeviceI2C(_address,id,'D');
    DistSensors[id] = dsensor;
    Wire.beginTransmission(_address);
    Wire.write(id);
    Wire.endTransmission();
  }

  void SkriBotI2C::AddLineSensor(int _address, byte id){
    DeviceI2C *lsensor = new DeviceI2C(_address,id,'L');
    LineSensors[id] = lsensor;
    Wire.beginTransmission(_address);
    Wire.write(id);
    Wire.endTransmission();
  }

  int SkriBotI2C::cti(char x){
  int y = x - '0';
  return(y);
}


  int SkriBotI2C::ReadLineSensor(byte id){ 
    char buff;
    for(int zz = 0; zz < N_LineSensors; zz++){
                    if(LineSensors[zz]->GetID() == id){
                      Wire.beginTransmission(LineSensors[zz]->GetAddress());
                      Wire.write('R');
                      Wire.endTransmission();
                        delay(100);
                        Wire.requestFrom(LineSensors[zz]->GetAddress(),1);
                        while (Wire.available()) { 
                        buff = Wire.read();
                        break;        
                        }  
                    }
      }
    
      return(cti(buff));
  }

   int SkriBotI2C::ReadDistSensor(byte id){ 
    char buff[3];
    for(int zz = 0; zz < N_LineSensors; zz++){
                    if(DistSensors[zz]->GetID() == id){

                      Wire.beginTransmission(DistSensors[zz]->GetAddress());
                      Wire.write('R');
                      Wire.endTransmission();
                        delay(100);
                        Wire.requestFrom(DistSensors[zz]->GetAddress(),3);
                        byte jj = 0;
                        while (Wire.available()) { 
                        buff[jj] = Wire.read();
                        jj++;
                        break;        
                        }  
                    }
      }
    
      return(cti(buff[0])*100 + cti(buff[1])*10 + cti(buff[0]));
  }


  
  void SkriBotI2C::Move(byte id){
      for(int zz = 0; zz < N_Rotors; zz++){
                    if(DCRotors[zz]->GetID() == id){
                      Wire.beginTransmission(DCRotors[zz]->GetAddress());
                      Wire.write('M');
                      Wire.endTransmission();
                    }
      }
  }


   /* void SkriBotI2C::TurnLeft(int ms){Move('Z',ms);}
    void SkriBotI2C::TurnRight(int ms){Move('M',ms);}
    void SkriBotI2C::FaceLeft(int ms){Move('L',ms);}
    void SkriBotI2C::FaceRight(int ms){Move('R',ms);}
    void SkriBotI2C::MoveForward(int ms){Move('F',ms);}
    void SkriBotI2C::MoveBack(int ms){Move('B',ms);}
    void SkriBotI2C::Stop(){Move('S',-1);}
    */


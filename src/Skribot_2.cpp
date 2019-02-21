#include "Skribot_2.h"
 
  Skribot_2::Skribot_2(){
    //Wire.begin();
    hspi->begin(); 
    vspi->begin();
    pinMode(hspi_CS, OUTPUT);
    BLE_Setup();
    #ifdef DEBUG_MODE
      Serial.begin(9600);
    #endif
  }

//HSPI functions (for CPLD communication)
byte Skribot_2::hspi_read(byte addr = 2){
  hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(hspi_CS, LOW);
  hspi->transfer(addr);
  byte out = hspi->transfer(0);
  digitalWrite(hspi_CS, HIGH);
  hspi->endTransaction();
  return(out);
}

void Skribot_2::hspi_write(byte value,byte addr) {
  hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(hspi_CS, LOW);
  hspi->transfer(addr);
  hspi->transfer(value);
  digitalWrite(hspi_CS, HIGH);
  hspi->endTransaction();
}

bool Skribot_2::Update_Module_Signals(){            
  
 if(  POWGD   == hspi_read(CPLD_POWGD) &&
      MODGD   == hspi_read(CPLD_MODGD)  &&
      MODPRES == hspi_read(CPLD_MODPRES)
  ){
  return(true);
 }else{ 

  POWGD   = hspi_read(CPLD_POWGD);
  MODGD   = hspi_read(CPLD_MODGD);
  MODPRES = hspi_read(CPLD_MODPRES);          
  if(MODGD & POWGD == MODPRES){
    #ifdef DEBUG_MODE
      Serial.println("All modules working.");
    #endif
  }else{
     #ifdef DEBUG_MODE
      Serial.println("Module fail.")
    #endif
  }
  return(false);
}
}
void Skribot_2::IdentifyModules(){
  for(byte ii = 0; ii < 8 ;ii++){
      if(bit_Read(MODGD,ii)){
        Set_module_CS(ii);
         SPITransfere(byte addr, byte *msg); // TODO
      }
  }
  
}

byte* SPITransfere(byte addr, byte *msg){
  Set_module_CS(addr);
  byte Nsend = msg[0] & B00001111;              //number of bytes to send
  byte Nrec  = (msg[0]>>4) & B00001111;         // nuber of bytes to recive
  byte output[Nrec];
  vspi->transfer(msg[1]);                       //transfering command address
  for(byte yy = 1; yy < Nsend+1;yy++){
    vspi->transfer(msg[yy+1]);                  //transfering to send data
  }
  byte ii = 0;
  for(byte zz = Nsend+1; zz<Nsend+Nrec+1;zz++){
    output[ii] = vspi->transfer(0);
    ii++;
  }
  Set_module_CS(0);
  return(output);
}

void Skribot_2::Set_module_CS(byte CSn){
  if(CSn != 0){
    hspi_write(1<<CSn,CPLD_SET_CS);
  }else{
    hspi_write(0,CPLD_SET_CS);
  }
}
//BLE FUNCTIONS (for mobile APP comunicaion) 
 char Skribot_2::BLE_read(){
   char tmp = BTmodule->BLE_read();
    return(tmp);
  }
  
  void Skribot_2::BLE_write(char *msg){
    BTmodule->BLE_write(msg);
  }

  bool Skribot_2::BLE_checkConnection(){
  bool connection;  
   connection = BTmodule->BLE_checkConnection();
  if(connection_Break_Reported){
    connection_Break_Reported = false;
    connection = false;
  }
    return(connection);
  }
  int Skribot_2::BLE_dataAvailable(){
    int dataAvalible = BTmodule->BLE_dataAvailable();
    return(dataAvalible);
  }

  void Skribot_2::wait_And_Check_BLE_Connection(int ms,int interval){
    int loop_iterator = ms/interval;
    int ms_left_after_loop = loop_iterator%interval;
    delay(ms_left_after_loop);
    for(int yy = 1; yy < loop_iterator; yy++){
          if(using_BLE_Connection && !connection_Break_Reported && BLE_checkConnection() == false){
          connection_Break_Reported = true;
        }else if(using_BLE_Connection && !program_End_Reported && BLE_dataAvailable() > 0){
            char tmp;
            while(BLE_dataAvailable() > 0){
              tmp = BLE_read();
              delay(5);                     // to be sure that next char will be recieved
              if((tmp == 'E' && BLE_read() == 'N' && BLE_read() == 'D') || (tmp == 'B' && BLE_read() == 'E' && BLE_read() == 'G')){
                program_End_Reported = true;
              }
            }
            if(program_End_Reported || connection_Break_Reported)break;
          
      }
      delay(interval);
    } 
    #ifdef DEBUG_MODE
    if(connection_Break_Reported)Serial.println("Connection LOST!");
    #endif
  }

void Skribot_2::BLE_Setup(){
    BTmodule = new BLEModule(ESP32_BLE);
    BTmodule->BLE_Setup();
    using_BLE_Connection = true;
  }

  void Skribot_2::BLE_reset(){
       while(BLE_checkConnection()){
          #ifdef DEBUG_MODE
          Serial.println("waiting...");
          #endif
        }
       BLE_Setup();
  }

  void Skribot_2::BLE_changeName(char name[], bool userConncection){
    BTmodule->BLE_changeName(name);
  }

/*
//I2C functions (not used now)
  void Skribot_2::ScanForDevices_I2C(){
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
            address_I2C[nDevices] = tmpaddress;
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


  void Skribot_2::IdentifyDevices_I2C(){

    char buff;
   for(int i = 0; i < nDevices ; i++){
     Wire.beginTransmission(address_I2C[i]);
     Wire.write('I');
     Wire.endTransmission();
      delay(100);
      Wire.requestFrom(address_I2C[i],1);
      int zz = 0;
      while(Wire.available()) { 
      buff = Wire.read();        
      }  
      delay(100); 

      switch(buff){
        case 'R':
            AddDCRotor(address_I2C[i],N_Rotors);
            Serial.print("LED ");
            Serial.print(N_Rotors);
            Serial.println(" added on address:");
            Serial.println(address_I2C[i]);
            N_Rotors++;
        break;
        case 'D':
            AddDistSensor(address_I2C[i],N_DistSensors);
            N_DistSensors++;
        break;
        case 'L':
            AddLineSensor(address_I2C[i],N_LineSensors);
          Serial.print("Line Sensor ");
            Serial.print(N_LineSensors);
            Serial.println(" added on address:");
            Serial.println(address_I2C[i]);
            N_LineSensors++;
        break;
        default:

        break;
      }       


   }

  }



  */

  //Aditional functions

byte Skribot_2::cti(char x){
   byte y = x - '0';
  return(y);
}

bool Skribot_2::bit_Read(byte in, byte n){
   return(in & (1<<n));
}


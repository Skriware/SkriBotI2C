#include "Skribot_2.h"
 
  Skribot_2::Skribot_2(){
    //Wire.begin();
    
  }

  void Skribot_2::setup(){
    BLE_Setup();
    //i2c_0 = new TwoWire(0);
    hspi = new SPIClass(HSPI);
    hspi->begin(14,12,13);
    Wire.begin();
    pinMode(CPLD_CS, OUTPUT);
    pinMode(CPLD_MOSI,OUTPUT);
    pinMode(CPLD_MISO,INPUT);
    pinMode(CPLD_SCK,OUTPUT);
    digitalWrite(CPLD_SCK,LOW);
    digitalWrite(CPLD_CS,HIGH);
    #ifdef DEBUG_MODE
      Serial.begin(230400);
    #endif
  }

byte Skribot_2::CPLD_read(byte addr){
  byte out = 0;
  digitalWrite(CPLD_CS, LOW);
  for (int i = 8; i != 0; i--)
  {
      digitalWrite (CPLD_MOSI, addr & (1 << (i-1) ));
      delayMicroseconds(CPLD_PERIOD/2);
      digitalWrite (CPLD_SCK, HIGH);
      delayMicroseconds(CPLD_PERIOD);
      digitalWrite (CPLD_SCK, LOW);
      delayMicroseconds(CPLD_PERIOD/2);
  }

  for (byte ii = 8; ii != 0; ii--)
  {   
    digitalWrite (CPLD_SCK, HIGH);
    delayMicroseconds(CPLD_PERIOD);
    out |= (digitalRead(CPLD_MISO) << (ii-1));                          
    digitalWrite (CPLD_SCK, LOW);
    delayMicroseconds(CPLD_PERIOD);   
  }
  return(out);
}

void Skribot_2::CPLD_write(byte value,byte addr){
  digitalWrite(CPLD_CS, LOW);
  for (int i = 8; i != 0; i--)
  {
    digitalWrite (CPLD_MOSI, addr & (1 << (i-1) ));
    delayMicroseconds(CPLD_PERIOD/2);
    digitalWrite (CPLD_SCK, HIGH);
    delayMicroseconds(CPLD_PERIOD);
    digitalWrite (CPLD_SCK, LOW);
    delayMicroseconds(CPLD_PERIOD/2);
  }
    for (int i = 8; i != 0; i--)
  {
    digitalWrite (CPLD_MOSI, value & (1 << (i-1) ));
    delayMicroseconds(CPLD_PERIOD/2);
    digitalWrite (CPLD_SCK, HIGH);
    delayMicroseconds(CPLD_PERIOD);
    digitalWrite (CPLD_SCK, LOW);
    delayMicroseconds(CPLD_PERIOD/2);
  }
    digitalWrite(CPLD_CS, HIGH);
}

byte Skribot_2::Header_byte(byte nSend,byte nRec){
  return(nSend | (nRec<<4));             //number of bytes to send        
}

void Skribot_2::Test_SPI_Comm(){
  byte mess[] = {Header_byte(3,3),3,1,2,3};
  SPITransfere(1,mess);
  Serial.println(output_buffer[0]);
  Serial.println(output_buffer[1]);
  Serial.println(output_buffer[2]);
}

void Skribot_2::Test_I2C_Comm(){
  byte mess[] = {Header_byte(3,3),3,1,2,3};
  I2CTransfere(14,mess);
  Serial.println(output_buffer[0]);
  Serial.println(output_buffer[1]);
  Serial.println(output_buffer[2]);
}

byte Skribot_2::Update_Module_Signals(){            
  byte _MODPRES,_POWGD,_MODGD;
  _MODPRES = CPLD_read(CPLD_MODPRES); 
  _POWGD   = CPLD_read(CPLD_POWGD);
  _MODGD   = CPLD_read(CPLD_MODGD);
  Serial.println(_MODPRES,BIN);
  Serial.println(_POWGD,BIN);
  Serial.println(_MODGD,BIN);
  if(_MODPRES == MODPRES && _POWGD == POWGD && _MODGD == MODGD){
  if(_MODGD == POWGD && POWGD== MODPRES){
    #ifdef DEBUG_MODE
      Serial.println("All modules working, and no module changes detected.");
    #endif
    return(MODULE_OK_CODE);
  }else{
         #ifdef DEBUG_MODE
        Serial.println("Module ERROR");
        #endif
    return(MODULE_ERROR_CODE);
  }
}else{
  MODGD = _MODGD;
  POWGD = _POWGD;
  MODPRES = _MODPRES;
if(MODGD == POWGD && POWGD== MODPRES){
   #ifdef DEBUG_MODE
    Serial.println("All modules working, module changes detected");
   #endif
    return(MODULE_CHANGED_CODE);
}else{
    #ifdef DEBUG_MODE
    Serial.println("Module ERROR.");
   #endif
  return(MODULE_ERROR_CODE);
}
}

}
void Skribot_2::IdentifyModules_SPI(){
if(Update_Module_Signals() != MODULE_ERROR_CODE){
  for(byte ii = 0; ii < connected_modules;ii++)delete modules[ii];
    connected_modules = 0;
  byte get_type_mess[] = {B00010000,1};
  for(byte ii = 0; ii < 8 ;ii++){
      byte tmp_id = 0;
      byte tmp_addr = ii+1;
      if(bit_Read(MODPRES,ii)){
         SPITransfere(tmp_addr,get_type_mess);           
         byte tmp_type = output_buffer[0];
         for(byte tt = 0; tt <connected_modules;tt++){
          if(tmp_type == modules[tt]->GetType())tmp_id++;
         }
         byte set_id_mess[] = {1,2,tmp_id};
         SPITransfere(tmp_addr,set_id_mess);
         if (connected_modules < 8){
          modules[connected_modules] = new Module(tmp_addr,tmp_id,tmp_type,CONNECTION_TYPE_SPI);
         connected_modules++;
         }else{
          Serial.println("All module slots occupied.");
         }
         #ifdef DEBUG_MODE
         Serial.println("Find module!");
         Serial.print("Type:");
         Serial.println(tmp_type);
         Serial.println("address:");
         Serial.println(tmp_addr);
         #endif
      }
  }
  }
}

byte Skribot_2::TransferAndReciveByte_SPI(byte in,byte addr){
   Set_module_CS(addr);
   hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));  
   byte out = hspi->transfer(in);
   hspi->endTransaction();
   Set_module_CS(0);
   return(out);
}


byte Skribot_2::SPITransfere(byte addr, byte *msg){
  for(byte p = 0; p < 15; p++)output_buffer[p] = 0;
  byte Nsend = msg[0] & B00001111;              //number of bytes to send
  byte Nrec  = (msg[0]>>4) & B00001111;         // nuber of bytes to recive
  byte output[Nrec];
  TransferAndReciveByte_SPI(msg[0],addr);           //transmit header
  TransferAndReciveByte_SPI(msg[1],addr);           //transfering command address
  for(byte yy = 1; yy < Nsend+1;yy++){
  TransferAndReciveByte_SPI(msg[yy+1],addr);
  }
  byte checksum = 0;
  for(byte jj = 0; jj < Nsend+2;jj++)checksum = checksum^msg[jj];
  checksum+=4;
  TransferAndReciveByte_SPI(checksum,addr);              //sending checksum
  for(byte zz = 0; zz<Nrec;zz++){
    output[zz] = TransferAndReciveByte_SPI(0,addr);             //receiving data 
    Serial.println("Got:");
    Serial.println(output[zz]);
  }
  byte tmp_checksum = TransferAndReciveByte_SPI(0,addr); 
    Serial.println("Checksum:");
    Serial.println(tmp_checksum); 
  byte rcv_checksum = 0;
  for(byte rr  = 0; rr <Nrec;rr++)rcv_checksum = rcv_checksum^output[rr];
    rcv_checksum+=4;
  if(tmp_checksum != rcv_checksum)Serial.println("SPI checksum error!");
  for(byte pp = 0; pp < Nrec; pp++)output_buffer[pp] = output[pp];
  return(Nrec);
}

void Skribot_2::Set_module_CS(byte CSn){
  if(CSn != 0){
    CPLD_write(1<<(CSn-1),CPLD_SET_CS);
  }else{
    CPLD_write(0,CPLD_SET_CS);
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
/*            I2C Functions */
 
    void Skribot_2::I2CSend(byte in,byte addr){
          Wire.beginTransmission(addr);
          Wire.write(in);
          Wire.endTransmission();
    }
    byte Skribot_2::I2CRecive(byte addr,byte size){
          byte out =0;
          Wire.requestFrom(addr,size);
          while(Wire.available()){ 
            out=Wire.read();        
          }
          return(out);

    }
    byte Skribot_2::I2CTransfere(byte addr, byte *msg){
           for(byte p = 0; p < 15; p++)output_buffer[p] = 0;
              byte Nsend = msg[0] & B00001111;              //number of bytes to send
              byte Nrec  = (msg[0]>>4) & B00001111;         // nuber of bytes to recive
              byte output[Nrec];
              I2CSend(msg[0],addr);           //transmit header
              I2CSend(msg[1],addr);           //transfering command address
              for(byte yy = 1; yy < Nsend+1;yy++){
                I2CSend(msg[yy+1],addr);
              }
              byte checksum = 0;
              for(byte jj = 0; jj < Nsend+2;jj++)checksum = checksum^msg[jj];
              checksum+=4;
              I2CSend(checksum,addr);              //sending checksum
              Wire.requestFrom(addr,Nrec+1);
              for(byte tt = 0; tt < Nrec;tt++){
                if(Wire.available())output[tt] = Wire.read();
              }
              byte tmp_checksum;
              if(Wire.available()){
                tmp_checksum = Wire.read();
              }else{
                tmp_checksum = 0;
              }
               byte rcv_checksum = 0;
              
              for(byte rr  = 0; rr <Nrec;rr++)rcv_checksum = rcv_checksum^output[rr];
              rcv_checksum+=4;
              if(tmp_checksum != rcv_checksum)Serial.println("I2C checksum error!");
              for(byte pp = 0; pp < Nrec; pp++)output_buffer[pp] = output[pp];
              return(Nrec);
    }

    void Skribot_2::IdentifyModules_I2C(){
     for(byte ii = 0; ii < connected_modules;ii++)delete modules[ii];
     connected_modules = 0;
     byte error;
     for(byte tmpaddress  = 0; tmpaddress < 127; tmpaddress++){  //I2C scan
        Wire.beginTransmission(tmpaddress);
        Wire.write(0xff);
        error = Wire.endTransmission();
      if (error == 0){
        Serial.print("I2C device found at address 0x");
        if (tmpaddress<16)
        Serial.print("0");
        Serial.print(tmpaddress,HEX);
        Serial.println("  !");
        
        byte get_type_mess[] = {B00010000,1};
        I2CTransfere(tmpaddress,get_type_mess);
        byte tmp_type = output_buffer[0];
        byte tmp_id = 0;
        Serial.print("Module Type: ");
        Serial.println(tmp_type);
        for(byte tt = 0; tt <connected_modules;tt++){
          if(tmp_type == modules[tt]->GetType())tmp_id++;
        }
        
        /*byte set_id_mess[] = {1,2,tmp_id};
        //I2CTransfere(tmpaddress,set_id_mess);
        if (connected_modules < 8){
          modules[connected_modules] = new Module(tmpaddress,tmp_id,tmp_type,CONNECTION_TYPE_I2C);
          connected_modules++;
        }*/
      }else if (error==4){
            Serial.print("Unknown error at address 0x");
            if (tmpaddress<16)
            Serial.print("0");
            Serial.println(tmpaddress,HEX);
      } 
      if(tmpaddress==127)Serial.println("No modules found.");   
    }
  }


  //Aditional functions

byte Skribot_2::cti(char x){
   byte y = x - '0';
  return(y);
}

bool Skribot_2::bit_Read(byte in, byte n){
   return(!(in & (1<<n)));
}


#ifndef SkriBotI2C_H
#define SkriBotI2C_H
#define DEBUG_MODE
#include "Arduino.h"
#include <Wire.h>
#include <utilities/Module.h>
#include <utilities/BLEModule.h>
#include <SPI.h>

//CPLD communication port:
#define CPLD_MOSI 18
#define CPLD_SCK  5
#define CPLD_MISO 19
#define CPLD_CS 17

#define CPLD_PERIOD 10
//CPLD ADDRESSES:
#define CPLD_POWGD 1 
#define CPLD_MODGD 2 
#define CPLD_MODPRES 3
#define CPLD_SET_CS 4 
#define CPLD_RESET 5

//MODULE SCAN ERROR CODES
#define MODULE_OK_CODE 0
#define MODULE_CHANGED_CODE 1
#define MODULE_ERROR_CODE  2
/* COMMON ADDRESSES:
1 - Identify module Type;
2 - Set Identifier;

*/
static const int spiClk = 100000; // 1 MHz

  class Skribot_2
 {
  public:
  	Skribot_2();  

    void setup();

    //byte TransfereAndRecive()

    void IdentifyModules_I2C();
    void I2CSend(byte in,byte addr);
    byte I2CRecive(byte addr,byte size);
    byte I2CTransfere(byte addr, byte *msg);

    byte TransferAndReciveByte_SPI(byte in, byte addr);
    byte SPITransfere(byte addr, byte *msg);
    void IdentifyModules_SPI();

    byte hspi_read(byte addr);
    void hspi_write(byte value,byte addr);
    
    void CPLD_write(byte value,byte addr);
    byte CPLD_read(byte addr);
    byte Update_Module_Signals();
    void Set_module_CS(byte CSn);

    char BLE_read();                                                        
    void BLE_write(char *msg);
    bool BLE_checkConnection();
    int  BLE_dataAvailable();
    void BLE_changeName(char name[],bool userConncection = true);
    void BLE_Setup();
    void BLE_reset();
    bool ProgramENDRepotred();
    byte cti(char x);
    bool bit_Read(byte in, byte n);
    void wait_And_Check_BLE_Connection(int ms,int interval);
    bool searchForModule(int id,int type);
    Module* getModule(byte type, byte id = 0);
    byte Header_byte(byte nSend,byte nRec);
    byte* Transfere(Module *target,byte *msg); 
    void Test_SPI_Comm();
    void Test_I2C_Comm();
   

    
   


  private:
  BLEModule *BTmodule;
  Module *modules[8];
  SPIClass * hspi = NULL;         // for CPLD communication
  SPIClass * vspi = NULL;         // for STM  communication
  TwoWire  * i2c_0 = NULL;
  byte POWGD;
  byte MODGD;
  byte MODPRES;
  byte connected_modules = 0;
  bool using_BLE_Connection,connection_Break_Reported, program_End_Reported,stausLEDused,high_power_usage;
  byte output_buffer[16];
 };

 #endif

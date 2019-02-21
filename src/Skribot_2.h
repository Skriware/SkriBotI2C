#ifndef SkriBotI2C_H
#define SkriBotI2C_H
//#define DEBUG_MODE
#include "Arduino.h"
#include <Wire.h>
#include <utilities/Module.h>
#include <utilities/BLEModule.h>
#include <SPI.h>
#define hspi_CS 15

//CPLD ADDRESSES:

#define CPLD_POWGD 1 
#define CPLD_MODGD 2 
#define CPLD_MODPRES 3
#define CPLD_SET_CS 4 
#define CPLD_RESET 5



static const int spiClk = 100000; // 1 MHz

  class Skribot_2
 {
  public:
  	Skribot_2();
    void ScanForDevices_I2C();            
    void IdentifyDevices_I2C();
    byte hspi_read(byte addr);
    void hspi_write(byte value,byte addr);
  

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
    //int BaterryCheck();       TODO



    byte* SPITransfere(byte addr, byte *msg);
    bool Update_Module_Signals();
    void IdentifyModules();
    void Set_module_CS(byte CSn);

  private:
  BLEModule *BTmodule;
  Module *modules[8];
  SPIClass * hspi = NULL;         // for CPLD communication
  SPIClass * vspi = NULL;         // for STM  communication
  byte POWGD;
  byte MODGD;
  byte MODPRES;
  bool using_BLE_Connection,connection_Break_Reported, program_End_Reported,stausLEDused,high_power_usage;
 };

 #endif

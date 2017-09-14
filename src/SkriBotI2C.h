#ifndef SkriBotI2C_H
#define SkriBotI2C_H
#include "Arduino.h"
#include "Arduino.h"
#include <Wire.h>
#include <utilities\DeviceI2C.h>

  class SkriBotI2C
 {
  public:
  	SkriBotI2C();

    void ScanForDevices();
    void IdentifyDevices();
    int cti(char x);

  	//void AddRotor(uint8_t Addr, int neutral = 1520); 
  	void AddDistSensor(int Addr, byte id);
    //void AddLED(uint8_t Addr);
    void AddLineSensor(int Addr, byte id); 
    //void AddScope(uint8_t Addr);
    void AddDCRotor(int Addr, byte id);
                                                                  //functions for element adding

    void Move(byte dir);

    int ReadDistSensor(byte id);
                                                                  //distance sensor readout
                            // LED functions

    int ReadLineSensor(byte id);
                                                                 // line sensor readout
     //int  GetScopeDistance(String id);                               //Scope functions
  private:
  DeviceI2C *SRotors[5];
  DeviceI2C *DistSensors[5];
  DeviceI2C  *LEDs[5];
  DeviceI2C *LineSensors[5];
  DeviceI2C *DCRotors[5];
  int N_Rotors;
  int N_DistSensors;
  int N_LED;
  int N_LineSensors;
  int N_Scopes;
  int DCSpeed = 0;
  int nDevices;
  int address[10];
 };

 #endif

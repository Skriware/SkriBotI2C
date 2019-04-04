#include <Skribot_2.h>

Skribot_2 *robot = new Skribot_2();
void setup() {
 
  robot->setup();
  byte yy=0;
  switch(yy){
    case 0:
    Serial.println(0);
    yy++;
    break;
    case 1:
    Serial.println(1);
    break;
  }

}

void loop() {
 if(Serial.available()){
    char input = Serial.read();
    if(input == 'B'){
     robot->IdentifyModules_SPI(); 
    }else if(input == 'C'){
    robot->IdentifyModules_I2C(); 
    }else{
    Serial.println(robot->TransferAndReciveByte_SPI(robot->cti(input),1));
    }
  }

}
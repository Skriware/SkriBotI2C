#include <Skribot_2.h>

Skribot_2 *robot = new Skribot_2();
void setup() {
 
  robot->setup();

}

void loop() {
 if(Serial.available()){
    char input = Serial.read();
    if(input == 'B'){
     robot->IdentifyModules_SPI(); 
    }else if(input == 'C'){
            Serial.println("I2C Scan");
    robot->IdentifyModules_I2C();

    }else if(input == 'T'){
    robot->Set_module_CS(1);
    delay(500);
    robot->Set_module_CS(0);
    }else{
    Serial.println(robot->TransferAndReciveByte_SPI(robot->cti(input),1));
    }
  }

}
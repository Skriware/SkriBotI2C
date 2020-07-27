#include <Skribot_2.h>

Skribot_2 *robot = new Skribot_2();
void setup() {
 
  robot->setup();
  Serial.println("Hello");

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

    }
  }

}

void run_Test_Program(){
    
}
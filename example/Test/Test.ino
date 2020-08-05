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
        run_Test_Program();
    }
  }

}

void run_Test_Program(){
    byte module_Types[] = {21,23,24};
    for(byte rr = 0;rr<3;rr++){
      if(!robot->searchForModule(module_Types[rr])){
        Serial.print("Missing module:");
        Serial.println(module_Types[rr]);
        return;
      }
    }
    Serial.println("All required modules detected!");
    Module *button = robot->getModule(0x18);
    Module *light_sensor = robot->getModule(0x15);
    Module *matrix = robot->getModule(0x17);
    byte read_button_msg[] = {0x11,0x0b,0};
    byte read_lux_msg[] = {0x20,0x0b};
    byte set_sun_msg[] = {0x08,0x0b,0x91, 0x52, 0x3c, 0x3c, 0xff, 0x3c, 0x4a, 0x89};
    byte set_moon_msg[] = {0x08,0x0b,0x38, 0x7c, 0x4e, 0x06, 0x06, 0x4e, 0x7e, 0x38};
    byte matrix_off_msg[] = {0x08,0x0b,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    byte matrix_reinit[] = {0x00,0xe0};
    robot->Transfere(matrix,matrix_reinit);
    
    while(true){
      robot->Transfere(light_sensor,read_lux_msg);
      int lux = robot->output_buffer[0];
      lux = lux << 8 | robot->output_buffer[1];  
      Serial.print("Light readout:");
      Serial.println(lux);
      if(lux > 50){
      robot->Transfere(matrix,set_sun_msg);
      }else{
      robot->Transfere(matrix,set_moon_msg);
      }

    if(Serial.available()){
      Serial.read();
       robot->Transfere(matrix,matrix_off_msg);
      break;
    }
  }

}
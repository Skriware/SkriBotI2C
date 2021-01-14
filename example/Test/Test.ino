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
    }else if(input == 'M'){
      run_Milestone_Test();
    }else if(input == 'G'){
      gas_sensor_test();
    }
  }

}

void run_Milestone_Test(){
  int succes_rate = 0;
  byte matrix_reinit[] = {0x00,0xe0};
  byte set_number_msg[] = {0x08,0x0b,0, 0, 0, 0, 0, 0, 0, 0};
  byte module_Types[] = {23,28};
  for(int hh = 0; hh < 3600*6;hh++){
    robot->IdentifyModules_SPI();
    if(robot->ChechModuleSetup(2,module_Types)){
      Serial.println("Matrix detected!");
      Module *matrix = robot->getModule(23);
      robot->Transfere(matrix,matrix_reinit);
      delay(50);
      robot->Transfere(matrix,set_number_msg);
      succes_rate++;
      set_number_msg[3] = 100*succes_rate/3600/6;
      delay(100);
    }
    Serial.println(succes_rate);
    if(Serial.available()){
            Serial.read();
             //robot->Transfere(buzzer,mute);
            break;
          }
  }
}

void gas_sensor_test(){
  robot->IdentifyModules_SPI();
  byte gas_sensor_id = 28;
  if(robot->ChechModuleSetup(1,&gas_sensor_id)){
    Serial.println("Gas sensor detected!");
    Module *gas_sensor = robot->getModule(28);
    byte gas_readout_msg[] = {0x20,0x0a};
    for(int tt = 0; tt<1000;tt++){
            robot->Transfere(gas_sensor,gas_readout_msg);
            int gas = robot->output_buffer[0];
            gas = gas << 8 | robot->output_buffer[1];  
            Serial.println(gas);
            delay(10);
    }
    
  }
}

void run_Test_Program(){
    byte module_Types[] = {0x18,0x15,0x17};
    byte buzzer_id = 0x16;
    if(robot->ChechModuleSetup(3,module_Types)){
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
  }else if(robot->ChechModuleSetup(1,&buzzer_id)){
    Module *buzzer = robot->getModule(0x16);
    byte  play_high_tone[] = {0x02,0x0a,int_to2byte(800,1),int_to2byte(800,0)};
    byte  play_low_tone[] = {0x02,0x0a,int_to2byte(400,1),int_to2byte(400,0)};
    byte  mute[] = {0x02,0x0a,0,0};
    while(true){
            robot->Transfere(buzzer,play_high_tone);
            delay(500);
            robot->Transfere(buzzer,play_low_tone);
            delay(500);
            
          if(Serial.available()){
            Serial.read();
             robot->Transfere(buzzer,mute);
            break;
          }
  }
  }
}

byte int_to2byte(int A,byte ii){
  byte tmp[2];
  tmp[0] = (A & 0xFF00) >> 8;
  tmp[1] = A & 0x00FF;
  Serial.println(A,BIN);
  Serial.println(tmp[0],BIN);
  Serial.println(tmp[1],BIN);
  return(tmp[ii]);
}
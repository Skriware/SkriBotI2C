#include <Skribot_2.h>

Skribot_2 *robot = new Skribot_2();
void setup() {
 
  robot->setup();
  Serial.println("Hello");
  /*for(byte rr = 0; rr<10;rr++){
    run_Milestone_Test_1();
    run_Milestone_Test_2();
    run_Milestone_Test_3_1();
    run_Milestone_Test_3_2();
    delay(3000);
     if(Serial.available()){
            Serial.read();         
            break;
     }
  }*/
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
    }else if(input == '1'){
      run_Milestone_Test_1();
    }else if(input == '2'){
      run_Milestone_Test_2();
    }else if(input == '3'){
      run_Milestone_Test_3_1();
    }else if(input == '4'){
      run_Milestone_Test_3_2();
    }
  }

}

byte val_to_bar(int val,int max){
  byte tmp_2[] = {1,2,4,8,16,32,64,128};
  if(val/(max/8) < 8){
    return(tmp_2[val/(max/8)]);
  }else{
    return(128);
  }
}

void run_Milestone_Test_3_1(){
  byte module_Types[] = {0x19,0x20};
  //robot->IdentifyModules_SPI();
  byte  recive_msg_IR[] = {0x10,0x0b};
  byte  set_servo_position[] = {0x01,0x0a,100};
    if(robot->ChechModuleSetup(2,module_Types)){
      Serial.println("Running v 3_1");
        long _time = millis();
        Module *servo = robot->getModule(0x20);
        Module *ir_rx = robot->getModule(0x19);
        while(true){
          robot->Transfere(ir_rx,recive_msg_IR);
          Serial.println(robot->output_buffer[0]);
          if(robot->output_buffer[0] == 2){
            set_servo_position[2] = 45;
          }else if(robot->output_buffer[0] == 3){
            set_servo_position[2] = 90;
          }
          robot->Transfere(servo,set_servo_position);
          delay(100);
          if(Serial.available()){
            Serial.read();
            
            break;
          }
          if((millis()-_time)%12000){
            Serial.println("Working for:");
            Serial.println(millis()-_time);
          }
        }
    }
}

void run_Milestone_Test_3_2(){
  byte module_Types[] = {0x19,0x11};
  //robot->IdentifyModules_I2C();
  byte  transmit_msg[] = {0x01,0x0a,2};
  byte  read_QTR[] = {0x21,0x0b,0};
    if(robot->ChechModuleSetup(2,module_Types)){
        Serial.println("Running v 3_2");
        long _time = millis();
        Module *qtr = robot->getModule(0x11);
        Module *ir_tx = robot->getModule(0x19);
        while(true){
          robot->Transfere(qtr,read_QTR);
          int raw_r = int(robot->output_buffer[0]<<8) | robot->output_buffer[1];
          Serial.println(raw_r);
          if(raw_r < 1500){
            transmit_msg[2] = 2;
            robot->Transfere(ir_tx,transmit_msg);
            delay(300);
          }else{
            transmit_msg[2] = 3;
            robot->Transfere(ir_tx,transmit_msg); 
            delay(300);
          }
          if(Serial.available()){
            Serial.read();
            break;
          }
          if((millis()-_time)%12000){
            Serial.println("Working for:");
            Serial.println(millis()-_time);
          }
        }
    }
}

void run_Milestone_Test_1(){
  byte module_Types[] = {0x1c,0x17,0x15,0x1a};
  //robot->IdentifyModules_I2C();
  byte matrix_reinit[] = {0x00,0xe0};
  byte set_state_msg[] = {0x08,0x0b,0, 0, 255, 0, 255,0, 0, 0};
  byte gas_readout_msg[] = {0x20,0x0a};
  byte read_lux_msg[] = {0x20,0x0b};
  byte get_red_msg[] = {0x41,0x0a,0};
  byte get_blue_msg[] = {0x41,0x0a,1};
  byte get_green_msg[] = {0x41,0x0a,3};
    if(robot->ChechModuleSetup(4,module_Types)){
        Serial.println("Running v 1");
        long _time = millis();
        Module *gas_sensor = robot->getModule(28);
        Module *matrix = robot->getModule(23);
        Module *light_sensor = robot->getModule(0x15);
        Module *color_sensor = robot->getModule(0x1a);
        robot->Transfere(matrix,matrix_reinit);
        while(true){
          robot->Transfere(light_sensor,read_lux_msg);    //light sesnor readout
          int lux = robot->output_buffer[0];
          lux = lux << 8 | robot->output_buffer[1];
          robot->Transfere(gas_sensor,gas_readout_msg);    //gas gensor readout
          int gas = robot->output_buffer[0];
          gas = gas << 8 | robot->output_buffer[1];
          robot->Transfere(color_sensor,get_red_msg);
          uint32_t red = int(robot->output_buffer[2]<<8) | robot->output_buffer[3];
          red = (uint32_t(robot->output_buffer[0]<<24) | uint32_t(robot->output_buffer[1]<<16)) | red;
          robot->Transfere(color_sensor,get_green_msg);
          uint32_t green = int(robot->output_buffer[2]<<8) | robot->output_buffer[3];
          robot->Transfere(color_sensor,get_blue_msg);
          uint32_t blue = int(robot->output_buffer[2]<<8) | robot->output_buffer[3];
          delay(100);
          //Serial.println(val_to_bar(blue,255));
          set_state_msg[2] = val_to_bar(lux,1000);
          set_state_msg[3] = val_to_bar(lux,1000);
          set_state_msg[5] = val_to_bar(gas,3000);
          set_state_msg[7] = val_to_bar(blue,40000);
          set_state_msg[8] = val_to_bar(red,40000);
          set_state_msg[9] = val_to_bar(green,40000);
          //delay(50);
          robot->Transfere(matrix,set_state_msg);
          delay(100);
          if(Serial.available()){
            Serial.read();
            break;
          }         
          if((millis()-_time) > 500){
            Serial.println("Working for:");
            Serial.println(millis()-_time);
            _time = millis();
            Serial.print("Readot:");
            Serial.print(":lux:");
            Serial.print(lux);
            Serial.print(":gas:");
            Serial.print(gas);
            Serial.print(":blue:");
            Serial.print(blue);
            Serial.print(":green:");
            Serial.print(green);
            Serial.print(":red:");
            Serial.println(red);
          }
        }
    }
}

void run_Milestone_Test_2(){
  byte module_Types[] = {0x16,0x18,0x1b};
  //robot->IdentifyModules_I2C();
  byte button_pressed_msg[] = {0x11,0x0d,0};
  byte button_1_pressed_msg[] = {0x11,0x0d,1};
  byte  play_high_tone[] = {0x02,0x0a,int_to2byte(600,1),int_to2byte(600,0)};
  byte  play_low_tone[] = {0x02,0x0a,int_to2byte(400,1),int_to2byte(400,0)};
  byte  mute[] = {0x02,0x0a,0,0};
  byte motion_readout_msg[] = {0x40,0x0a};
    if(robot->ChechModuleSetup(3,module_Types)){
        Serial.println("Running v 2");
        long start_time = millis();
        long _time = millis();
        Module *motion_sensor = robot->getModule(0x1b);
        Module *button = robot->getModule(0x18);
        Module *buzzer = robot->getModule(0x16);
        byte tmp_button = 0;
        byte tmp_button_1 = 0;
        int dx = 0;
        int dy = 0;
        while(true){
          robot->Transfere(button,button_pressed_msg);
          tmp_button = robot->output_buffer[0];
          robot->Transfere(button,button_1_pressed_msg);
          tmp_button_1 = robot->output_buffer[0];
          //Serial.print(tmp_button);
          //Serial.println(tmp_button_1);
          if(tmp_button ==0 || tmp_button_1==0){
            robot->Transfere(motion_sensor,motion_readout_msg);
            dx = (int)(robot->output_buffer[0]<<8) | robot->output_buffer[1];
            dy = (int)(robot->output_buffer[2]<<8) | robot->output_buffer[3];        
            if(dx > 1000 || dy >1000){
              for(byte rr =0;rr<10;rr++){
              robot->Transfere(buzzer,play_low_tone);
              if(tmp_button_1 == 0){
                delay(10);
              }else{
                delay(30);
              }
              robot->Transfere(buzzer,play_high_tone);
              if(tmp_button_1 == 0){
                delay(10);
              }else{
                delay(30);
              }
              }
            }else{
              robot->Transfere(buzzer,mute);
            }
          }
          robot->Transfere(buzzer,mute);
          if(Serial.available()){
            Serial.read();
            robot->Transfere(buzzer,mute);
            break;
          }
          if((millis()-_time) >600000){
            Serial.println("Working for:");
            Serial.println(millis()-start_time);
            _time = millis();
            Serial.print("Button:");
            Serial.print(tmp_button);
            Serial.print(tmp_button_1);
            Serial.print(":X:");
            Serial.print(dx);
            Serial.print("");
            Serial.print(":Y:");
            Serial.println(dy);      
          }
        }
    }
}

void run_Milestone_Test(){
  int succes_rate = 0;
  byte matrix_reinit[] = {0x00,0xe0};
  byte set_number_msg[] = {0x08,0x0b,0, 0, 0, 0, 0, 0, 0, 0};
  byte module_Types[] = {23,28};
  for(int hh = 0; hh < 3600*6;hh++){
    robot->IdentifyModules_I2C();
    if(robot->ChechModuleSetup(2,module_Types)){
      Serial.println("Matrix detected!");
      long _time = millis();
      Module *matrix = robot->getModule(23);
      robot->Transfere(matrix,matrix_reinit);
      delay(50);
      robot->Transfere(matrix,set_number_msg);
      succes_rate++;
      set_number_msg[3] = succes_rate;
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
  robot->IdentifyModules_I2C();
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
#include <SkriBotI2C.h>

SkriBotI2C *robot = new SkriBotI2C();
void setup() {
  Serial.begin(9600);
  Serial.println("lopl");
   robot->ScanForDevices();
   robot->IdentifyDevices();
   robot->Move(0);
   robot->Move(1);
   
   while(true){
   Serial.println(robot->ReadLineSensor(0));
      delay(500);
   }
  // put your setup code here, to run once:

}

int cti(char x){
  int y = x - '0';
  return(y);
}

void loop() {
 if(Serial.available()){
  int i = 0;
  char buff[10];
  while(Serial.available()){
  buff[i] = Serial.read();
  i++;
 }
  
int id;
  switch(buff[0]){
    case 'X' :
     Serial.println("Operate LED:");
      id = cti(buff[1]);
      robot->Move(cti(id));
      Serial.print("LED ");
      Serial.print(id);
      Serial.println("operated!");
      
    break;
    case 'L':
        Serial.println("Operate Line Sensor give ID:");
          Serial.print("Readout from line sensor:");
         // Serial.println(robot->ReadLineSensor(cti(id)));
        
      break;

    default:

     break;
  }
 }

}

#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
String devices[10][4];

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setBacklight(5);
  bool sync = false;
  while(! sync){
    Serial.write("Q"); // Send "Q" through the serial port
    
    if (Serial.available() > 0) { // Check if there is any data received
      char incoming = Serial.read(); // Read the incoming data

      if (incoming == 'X') { // Check if the incoming data is "X"
        sync = true;
      }
    }
    delay(1000); // Wait for 1 second
  }
  Serial.write("BASIC\n");
  lcd.setBacklight(7);
}

int operation(String str){
  if(str == "" || str[1] != 45){ //check dash
    return 4;
  } else if(str[0] == 65) { //check A
    return 0;
  } else if(str[0]== 83){ //check S
    return 1;
  }else if(str[0]== 80){ //check P
    return 2;
  } else if(str[0] == 82){ //check R
    return 3;
  } else{
    return 4;
  }
}

int find_row(String arr[10][4], String searchTerm) {
  for(int i=0; i<10; i++) {
    if(arr[i][0] == searchTerm) { // searches first term f each row, corresponding to device id
      return i;
    }
  }
  return -1;
}

void add_device(String str){
  //Serial.print("add called");
  String temp_dev_id = str.substring(2,5); //isolates device id
  int location = find_row(devices, temp_dev_id); //checks if device already exists
  if(location == -1){
    location = find_row(devices,"");//checks for empty space
  }
  if(location == -1){
    Serial.print("ERROR:devices list full, please remove device before adding any more");//error too many devices
  } else{
    if(str[5] == 45 && str[7] == 45){
      //Serial.print("check succ");
      String temp_dev_type = str.substring(6,7);
      //Serial.print(temp_dev_id);
      String temp_dev_location = str.substring(8,22);
      devices[location][0] = temp_dev_id;
      devices[location][1] = temp_dev_type;
      devices[location][2] = temp_dev_location;
      devices[location][3] = "OFF";
    } 
  }

}

void change_state(String str){
  //changestatestuff
}

#define WAITING 0
#define RECIEVING_DATA 1
#define PROCESSING_DATA 2
#define BUTTON_INPUTS 3
#define INPUTS_HANDELING 4
int state = WAITING;

void loop(){
  delay(1000);
  switch (state){
    case WAITING:
      //Serial.print("W");
      if(Serial.available() > 0){
        state = RECIEVING_DATA;
      }
      break;
    case RECIEVING_DATA:
      //Serial.print("R");
      if(Serial.available() > 0){
        String incomingString = Serial.readString();
        int temp_operation = operation(incomingString);
        if (temp_operation == 0){
          add_device(incomingString);
        }
        Serial.print(devices[0][0]);
        state = RECIEVING_DATA;
      } else if (Serial.available() == 0){
        state = WAITING;
      }
      break;
  }
}
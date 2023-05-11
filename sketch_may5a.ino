#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
String devices[10][4];
int displaying = 0;
char row1[23], row2[16];
unsigned long lastChecked;

byte customChar[8] = {
  0b00111,
  0b00101,
  0b00111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.createChar(0, customChar);
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
  lastChecked = millis();
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
  for(int i=0; i<9; i++) {
    if(arr[i][0] == searchTerm) { // searches first term f each row, corresponding to device id
      return i;
    }
  }
  return -1;
}

void add_device(String str){
  //Serial.print("add called");
  String temp_dev_id = str.substring(2,5); //isolates device id
  String temp_dev_type = str.substring(6,7);//isolates device type
  int location = find_row(devices, temp_dev_id); //checks if device already exists
  if(location == -1){
    location = find_row(devices,"");//checks for empty space
  }
  if(location == -1){
    Serial.println(F("ERROR:devices list full, please remove device before adding any more"));//error too many devices
  } else{
    if(str[5] == 45 && str[7] == 45){
      //Serial.print("check succ");
      //Serial.print(temp_dev_id);
      String temp_dev_location = str.substring(8,22);
      devices[location][0] = temp_dev_id;
      devices[location][1] = temp_dev_type;
      devices[location][2] = temp_dev_location;
      devices[location][3] = "OFF";
      if(temp_dev_type == "T"){
        devices[location][4] = "8";
      }else if(temp_dev_type =="S" || temp_dev_type =="L"){
        devices[location][4] = "0";
      }
      //Serial.print(location);
      //Serial.print(devices[0][0]);
      //Serial.print(devices[0][1]);
      //Serial.print(devices[location][2]);
      //Serial.print(devices[location][3]);
      //Serial.print(devices[location][2] + devices[location][3]);
    } 
  }

}

void change_state(String str){
  //Serial.print("ran cs");
  String temp_dev_id = str.substring(2,5); //isolates device id
  String temp_dev_state = str.substring(6,9);
  //Serial.print(temp_dev_id);
  //Serial.print(temp_dev_state);
  int location = find_row(devices, temp_dev_id);
  if(location == -1){
    Serial.println(F("ERROR:device does not exist"));
  } else if(str[5] == 45 &&(temp_dev_state == "ON" || temp_dev_state == "OFF")){
    //Serial.print("checkpas");
    devices[location][3] = temp_dev_state; 
  }
}

void change_power(String str){
  Serial.print("ran cp ");
  String temp_dev_id = str.substring(2,5);
  String temp_dev_power = str.substring(6,9);
  int location = find_row(devices, temp_dev_id);
  if(location == -1){ 
    Serial.print("ERROR:device does not exist");
  } else if (str[5] == 45 && devices[location][1] != "O" && devices[location][1] != "C"){
    Serial.print("passed OC check");
    if (devices[location][1] != "T" && atoi(temp_dev_power.c_str()) > 8 && atoi(temp_dev_power.c_str()) < 33){
      devices[location][4] = temp_dev_power;
      Serial.print("passed 8 33 test ");
    } else if((devices[location][1] != "S" || devices[location][1] != "L") && atoi(temp_dev_power.c_str()) > -1 && atoi(temp_dev_power.c_str()) < 101){
      devices[location][4] = temp_dev_power;
      Serial.print("passed 0 100 test ");
    }else{
      Serial.println(F("ERROR:power out of range"));  
    }
  } else{
    Serial.println(F("ERROR:device has no power setting"));
  }
}

void remove_device(String str){
  String temp_dev_id = str.substring(2,5);
  int location = find_row(devices, temp_dev_id);
  if(location == -1){ 
    Serial.print("ERROR:device does not exist");
  } else{
    devices[location][0] = "";
    devices[location][1] = "";
    devices[location][2] = "";
    devices[location][3]= "";
    devices[location][4]= "";
  }

}

#define WAITING 0
#define RECIEVING_DATA 1
#define BUTTON_INPUTS 2
int state = WAITING;

void loop(){
  //Serial.print(displaying);
  switch (state){
    case WAITING:{
      if((lastChecked + 350) < millis()){
        lastChecked = millis();
        String dev_state = devices[displaying][3];
        String state_out = "";
        if(dev_state == "OFF"){
          lcd.setBacklight(3);
          state_out = "OFF";
        } else if(dev_state == "ON"){
          lcd.setBacklight(2);
          state_out = " ON";
        }
        //Serial.print("W");
        lcd.clear();
        sprintf(row1,"%s%s %s", (displaying == 0)? " " : "^", (devices[displaying][0] == "")? "   " : devices[displaying][0].c_str(), (devices[displaying][2] == "")? "           " : devices[displaying][2].c_str());
        if(devices[displaying][1] == "T"){
          sprintf(row2,"%s%s %s %s  C",(displaying == 9)? " " : "v",(devices[displaying][1] == "")? " " : devices[displaying][1].c_str(),(devices[displaying][3] == "")? "   " : state_out.c_str(), (devices[displaying][4] == "")? "    " : devices[displaying][4].c_str());
        } else if(devices[displaying][1] == "S" || devices[displaying][1] == "L"){
          sprintf(row2,"%s%s %s %s%%",(displaying == 9)? " " : "v",(devices[displaying][1] == "")? " " : devices[displaying][1].c_str(), (devices[displaying][3] == "")? "   " : state_out.c_str(), (devices[displaying][4] == "")? "    " : devices[displaying][4].c_str());
        } else{
          sprintf(row2,"%s%s %s",(displaying == 9)? " " : "v",(devices[displaying][1] == "")? " " : devices[displaying][1].c_str(), (devices[displaying][3] == "")? "   " : state_out.c_str());
        }
        lcd.setCursor(0,0);
        lcd.print(row1);
        //Serial.print(row1);
        lcd.setCursor(0,1);
        lcd.print(row2);
        if(devices[displaying][1] == "T"){
          lcd.setCursor(9,1);
          lcd.write((byte)0);  
        }
        //Serial.print(row2);
        if(Serial.available() > 0){
          state = RECIEVING_DATA;
        }else if(lcd.readButtons()!= 00000000){
          state = BUTTON_INPUTS;
        }
    }
    }break;
    case RECIEVING_DATA:{
      //Serial.print("R");
      if(Serial.available() > 0){
        String incomingString = Serial.readString();
        int temp_operation = operation(incomingString);
        if (temp_operation == 0){
          add_device(incomingString);
        } else if (temp_operation ==1){
          change_state(incomingString);
        } else if (temp_operation == 2){
          change_power(incomingString);  
        }else if(temp_operation == 3){
          remove_device(incomingString);
        }else{
          Serial.println(F("ERROR:incorrect instruction format"));
        }
        //Serial.print(devices[0][0]);
        state = RECIEVING_DATA;
      } else if (Serial.available() == 0){
        state = WAITING;
      }
    }break;
    case BUTTON_INPUTS:{
    //Serial.print("button");
    uint8_t pressedButtons = lcd.readButtons();
    if ((BUTTON_UP & pressedButtons) && displaying > 0){
        lastChecked = millis();
      --displaying;
    } else if((BUTTON_DOWN & pressedButtons) && displaying < 9){
        lastChecked = millis();
      ++displaying;
      }
    //if(lcd.readButtons()== 00000000){
      state = WAITING;
      //}
    }break;
  }
}

#include <Wire.h>

String devices[10][4];

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
  String temp_dev_id = str.substring(2,4);
  int location = find_row(devices, temp_dev_id);
  if(location == -1){
    location == find_row(devices,"");
  }
  if(location == -1){
    Serial.print("ERROR:devices list full, please remove device before adding any more");//error too many devices
  } else{
    if(str[5] == str[7] == "-"){
      String temp_dev_type = str.substring(6,6);
      String temp_dev_location = str.substring(8,22);
      devices[location][0] = temp_dev_id;
      devices[location][1] = temp_dev_type;
      devices[location][2] = temp_dev_location;
      devices[location][3] = "OFF";
    } 
  }

}

void setup() {
  Serial.begin(9600);
  String test = "R-test";

  Serial.print(find_row(devices,""));
}

void loop() {

}

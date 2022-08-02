#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

struct Full_Data{
  int joystick_xAxis;
  int joystick_yAxis;
  int joystick_pitch;
  int joystick_roll;
  byte joystick_toggleSwitch1;
  byte joystick_toggleSwitch2;
  byte joystick_id; 
};
Full_Data data = {500, 500, 0, 0, 1, 1, 0};

String previousDirection = "dur";
String currentDirection = "dur";
boolean directionTypeMPU6050 = false;

boolean loadState = false;  //Toggle Switch-2 1 iken false yani inik durum.
byte prevLoad = 1;
unsigned long timer = 0;

void show_nrf_data();
void Direction_Control();
void MPU6050_Direction_Control();
void loadControl();

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(false);
  //radio.powerUp();
  //radio.powerDown();
  radio.startListening();
  pinMode(2, OUTPUT);
  digitalWrite(2,LOW);
  delay(2000);
  Serial.println("nRF24 Receiver is ready to listening.");
  show_nrf_data();
}

void loop() {
  // Check whether there is data to be received
  if (radio.available()) {  //Gelen Verinin ID'si 111 ise işleme alacak!
    radio.read(&data, sizeof(Full_Data)); // Read the whole data and store it into the 'data' structure
    if(data.joystick_id == 111){
      digitalWrite(2, HIGH);
      delay(50);
      digitalWrite(2, LOW);
      if(data.joystick_toggleSwitch1 == 0){
        directionTypeMPU6050 = true;
      }
      else{
        directionTypeMPU6050 = false;
      }
      if(directionTypeMPU6050 == false){
        Direction_Control();
      }
      else{
        MPU6050_Direction_Control();
      }
      loadControl();
      show_nrf_data();
    }
  }
}

void show_nrf_data(){
  Serial.println("nRF24L01 - Joystick Data Is; ID: " + String(data.joystick_id));
  Serial.print("Roll: " + String(data.joystick_roll) + " | Pitch: " + String(data.joystick_pitch));
  Serial.print(" | xAxis: " + String(data.joystick_xAxis) + " | yAxis: " + String(data.joystick_yAxis));
  Serial.println(" | Toggle Switch-1: " + String(data.joystick_toggleSwitch1) + " | Toggle Switch-2: " + String(data.joystick_toggleSwitch2));
  Serial.println("-----------------------------------------------------------------------------------------");
}

void Direction_Control(){
  if(data.joystick_xAxis < 100 && data.joystick_yAxis < 100){
    currentDirection = "#saggeri";
   }
   else if(data.joystick_xAxis < 100 && data.joystick_yAxis > 900){
    currentDirection = "#sagileri";
   }
   else if(data.joystick_xAxis > 900 && data.joystick_yAxis < 100){
    currentDirection = "#solgeri";
   }
   else if(data.joystick_xAxis > 900 && data.joystick_yAxis > 900){
    currentDirection = "#solileri";
   }
   else if(data.joystick_xAxis < 100){
    currentDirection = "sag";
   }
   else if(data.joystick_xAxis > 900){
    currentDirection = "sol";
   }
   else if(data.joystick_yAxis < 100){
    currentDirection = "geri";
   }
   else if(data.joystick_yAxis > 900){
    currentDirection = "ileri";
   }
   else{
    currentDirection = "dur";
   }
  
   if(currentDirection != previousDirection){
     Serial.println(currentDirection);
     previousDirection = currentDirection; 
    }
}

void MPU6050_Direction_Control(){ //Toggle Switch-1 MPU6050 ile yön kontrolünü aktif edecek!
  if(data.joystick_pitch >= 30){   //Toggle Switch-1 0 ise MPU6050 ile yön kontrol aktif olacak!
    currentDirection = "ileri";
  }
  else if(data.joystick_pitch <= -30){
    currentDirection = "geri";
  }
  else if(data.joystick_roll <= -30){
    currentDirection = "sol";
  }
  else if(data.joystick_roll >= 30){
    currentDirection = "sag";
  }
  else{ //Joystick kontrol ile çakışabilir!
    currentDirection = "dur";
  }
  if(currentDirection != previousDirection){
     Serial.println(currentDirection);
     previousDirection = currentDirection; 
    }
}

//byte prevButton1 = 1;
//byte prevButton2 = 1;
//boolean right_button1 = false;
//boolean right_button2 = false;

void loadControl(){
  //loadState
  if(data.joystick_toggleSwitch2 != prevLoad){
    prevLoad = data.joystick_toggleSwitch2;
    if(loadState == false){
      Serial.println("yukal");
      loadState = true;
      timer = millis();
      while((millis() - timer) < 10000);
      Serial.println("yukoke");
      delay(50);
      Serial.println("yukoke");
    }
    else{
      Serial.println("yukbirak");
      loadState = false;
      timer = millis();
      while((millis() - timer) < 10000);
      Serial.println("yukoke");
      delay(50);
      Serial.println("yukoke");
    }
  }
}

//nRF24L01 lib
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//MPU6050 lib
#include <MPU6050.h>

//nRF24L01 Değişkenler
RF24 radio(5, 6);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001"; // Address
byte number = 0;

void nrf24_Init(void);

#define Joystick_X_Pin A2 
#define Joystick_Y_Pin A3

const byte potSensvity = 50;

int joystick_right_x = 500;
int joystick_right_y = 500;
int joystick_right_x_prev = 0;
int joystick_right_y_prev = 0;

int joystick_left_x = 500;
int joystick_left_y = 500;
int joystick_left_x_prev = 500;
int joystick_left_y_prev = 500;

struct Kumanda{
  int xAxis;
  int yAxis;
};
Kumanda kumanda = {500, 500};

//MPU6050 Değişkenler
MPU6050 mpu;

const byte MPU_Sensvity = 10;

int pitch = 0;
int roll = 0;
int pitch_prev = 0;
int roll_prev = 0;

struct MPU6050_Struct{
  int pitch;
  int roll;
};

MPU6050_Struct mpu6050 = {0, 0};

//Toggle Switch
#define Toggle_Switch1_Pin 7
#define Toggle_Switch2_Pin 4

//Push Button
#define Right_Button1 2
#define Right_Button2 3
byte prevButton1 = 1;
byte prevButton2 = 1;
boolean right_button1 = false;
boolean right_button2 = false;

struct Full_Data{
  int joystick_xAxis;
  int joystick_yAxis;
  int joystick_pitch;
  int joystick_roll;
  byte joystick_toggleSwitch1;
  byte joystick_toggleSwitch2;
  byte joystick_id; 
};
Full_Data data = {500, 500, 0, 0, 1, 1, 111};

void joystick_control();
void directionControl();
void toggleSwitchControl();
void button_control();
void show_nrf_data();

void setup() {
  Serial.begin(9600);
  pinMode(Toggle_Switch1_Pin, INPUT_PULLUP);
  pinMode(Toggle_Switch2_Pin, INPUT_PULLUP);
  //pinMode(Right_Button1, INPUT_PULLUP);
  //pinMode(Right_Button2, INPUT_PULLUP);
  //nrf24_Init();
  Serial.println("nRF24L01 Baslatiliyor");
  boolean state = radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(false);
  radio.stopListening();
  
  if(state == true){
    Serial.println("nRF24-SPI Communication Started.");
  }
  else{
    Serial.println("nRF24-SPI Communication Failled!");
    //while(1);
  }
  delay(500);
  Serial.println("Initialize MPU6050");
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  Serial.println("MPU6050-I2C Communication Started");
  delay(500);
  Serial.println("Verici Başlatıldı...");
}

void loop() {
  joystick_control();
  directionControl();
  toggleSwitchControl();
  //button_control();
  delay(200);
}

void nrf24_Init(){
  Serial.println("nRF24L01 Baslatiliyor");
  boolean state = radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();
  
  if(state == true){
    Serial.println("nRF24-SPI Communication Started.");
  }
  else{
    Serial.println("nRF24-SPI Communication Failled!");
    //while(1);
  }
  delay(1000);
  Serial.println("Verici Başlatıldı...");
}

void joystick_control(){
  int joystick_right_x = analogRead(A2);
  int joystick_right_y = analogRead(A3);

  if(!((joystick_right_x < (joystick_right_x_prev + potSensvity)) && (joystick_right_x > (joystick_right_x_prev - potSensvity)))){
    //Serial.println("," + String(joystick_right_x) + "," + String(joystick_right_y));
    kumanda.xAxis = joystick_right_x;
    kumanda.yAxis = joystick_right_y;
    data.joystick_xAxis = joystick_right_x;
    data.joystick_yAxis = joystick_right_y;
    radio.write(&data, sizeof(Full_Data));
    show_nrf_data();
    joystick_right_x_prev = joystick_right_x;
    joystick_right_y_prev = joystick_right_y;
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
  }
  if(!((joystick_right_y < (joystick_right_y_prev + potSensvity)) && (joystick_right_y > (joystick_right_y_prev - potSensvity)))){
    //Serial.println("," + String(joystick_right_x) + "," + String(joystick_right_y));
    kumanda.xAxis = joystick_right_x;
    kumanda.yAxis = joystick_right_y;
    data.joystick_xAxis = joystick_right_x;
    data.joystick_yAxis = joystick_right_y;
    radio.write(&data, sizeof(Full_Data));
    show_nrf_data();
    joystick_right_x_prev = joystick_right_x;
    joystick_right_y_prev = joystick_right_y;
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
  }
  //Serial.println("Joystick Right X: " + String(joystick_right_x));
  //Serial.println("Joystick Right Y: " + String(joystick_right_y));
}

void directionControl(){
  Vector normAccel = mpu.readNormalizeAccel();

  // Calculate Pitch & Roll
  pitch = -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis*normAccel.YAxis + normAccel.ZAxis*normAccel.ZAxis))*180.0)/M_PI;
  roll = (atan2(normAccel.YAxis, normAccel.ZAxis)*180.0)/M_PI;

  if(!((pitch < (pitch_prev + MPU_Sensvity)) && (pitch > (pitch_prev - MPU_Sensvity)))){
    mpu6050.pitch = pitch;
    mpu6050.roll = roll;
    data.joystick_pitch = pitch;
    data.joystick_roll = roll;
    if(pitch > 25 || pitch < -25){
      radio.write(&data, sizeof(Full_Data));
      show_nrf_data();
    }
    else if(pitch < 10 || pitch > -10){
      radio.write(&data, sizeof(Full_Data));
      show_nrf_data();
    }
    //Serial.println("MPU6050 Pitch: " + String(mpu6050.pitch) + " | Roll: " + String(mpu6050.roll));
    //radio.write(&kumanda, sizeof(Kumanda));
    pitch_prev = pitch;
    roll_prev = roll;
  }

  if(!((roll < (roll_prev + MPU_Sensvity)) && (roll > (roll_prev - MPU_Sensvity)))){
    mpu6050.pitch = pitch;
    mpu6050.roll = roll;
    data.joystick_pitch = pitch;
    data.joystick_roll = roll;
    if(roll > 25 || roll < -25){
      radio.write(&data, sizeof(Full_Data));
      show_nrf_data();
    }
    else if(roll < 10 || roll > -10){
      radio.write(&data, sizeof(Full_Data));
      show_nrf_data();
    }
    //Serial.println("MPU6050 Pitch: " + String(mpu6050.pitch) + " | Roll: " + String(mpu6050.roll));
    //radio.write(&kumanda, sizeof(Kumanda));
    pitch_prev = pitch;
    roll_prev = roll;
  }
}

void toggleSwitchControl(){
  byte toggleSwitch2 = digitalRead(Toggle_Switch2_Pin);
  if(data.joystick_toggleSwitch2 != toggleSwitch2){
    data.joystick_toggleSwitch2 = toggleSwitch2;
    radio.write(&data, sizeof(Full_Data));
    show_nrf_data();
    delay(100);
  }
  byte toggleSwitch1 = digitalRead(Toggle_Switch1_Pin);
  if(data.joystick_toggleSwitch1 != toggleSwitch1){
    data.joystick_toggleSwitch1 = toggleSwitch1;
    radio.write(&data, sizeof(Full_Data));
    show_nrf_data();
    delay(100);
  }
}

void show_nrf_data(){
  Serial.println("nRF24L01 - Joystick Data Is; ID: " + String(data.joystick_id));
  Serial.print("Roll: " + String(data.joystick_roll) + " | Pitch: " + String(data.joystick_pitch));
  Serial.print(" | xAxis: " + String(data.joystick_xAxis) + " | yAxis: " + String(data.joystick_yAxis));
  Serial.println(" | Toggle Switch-1: " + String(data.joystick_toggleSwitch1) + " | Toggle Switch-2: " + String(data.joystick_toggleSwitch2));
  Serial.println("-----------------------------------------------------------------------------------------");
}

void button_control(){
  byte button1 = digitalRead(Right_Button1);
  if(button1 != prevButton1){ //Değişim olduğunda tetiklenecek
    //delay(100);
    prevButton1 = button1;
    if(button1 == 0){
      Serial.println("Button-1 Pressed");
    }
    else{
      Serial.println("Button-1 Released");
    }
  }
//  if(button1 == 0){  //Buton-1'e Basıldıysa
//    //while(digitalRead(Right_Button1) == LOW);
//    delay(100);
//    Serial.println("Button-1 Pressed");
//  }
//  else{
//    
//  }
//  byte button2 = digitalRead(Right_Button2);
//  if(button2 == 0){  //Buton-2'ye Basıldıysa
//    //while(digitalRead(Right_Button2) == LOW);
//    delay(100);
//    Serial.println("Button-2 Pressed");
//  }
//  else{
//    
//  }
}

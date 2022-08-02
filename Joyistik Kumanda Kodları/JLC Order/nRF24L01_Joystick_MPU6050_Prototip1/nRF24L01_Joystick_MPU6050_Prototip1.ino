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

const byte MPU_Sensvity = 3;

int pitch = 0;
int roll = 0;
int pitch_prev = 0;
int roll_prev = 0;

struct MPU6050_Struct{
  int pitch;
  int roll;
};

MPU6050_Struct mpu6050 = {0, 0};

struct Full_Data{
  int joystick_xAxis;
  int joystick_yAxis;
  int joystick_pitch;
  int joystick_roll;
};
Full_Data data = {500, 500, 0, 0};

void joystick_control();
void directionControl();
void show_nrf_data();

void setup() {
  Serial.begin(9600);
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
    delay(100);
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
    delay(100);
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
    radio.write(&data, sizeof(Full_Data));
    show_nrf_data();
    //Serial.println("MPU6050 Pitch: " + String(mpu6050.pitch) + " | Roll: " + String(mpu6050.roll));
    //radio.write(&kumanda, sizeof(Kumanda));
    pitch_prev = pitch;
    roll_prev = roll;
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
  }

  if(!((roll < (roll_prev + MPU_Sensvity)) && (roll > (roll_prev - MPU_Sensvity)))){
    mpu6050.pitch = pitch;
    mpu6050.roll = roll;
    data.joystick_pitch = pitch;
    data.joystick_roll = roll;
    radio.write(&data, sizeof(Full_Data));
    show_nrf_data();
    //Serial.println("MPU6050 Pitch: " + String(mpu6050.pitch) + " | Roll: " + String(mpu6050.roll));
    //radio.write(&kumanda, sizeof(Kumanda));
    pitch_prev = pitch;
    roll_prev = roll;
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
  }
}

void show_nrf_data(){
  Serial.println("Joystick Roll: " + String(data.joystick_roll) + " | Pitch: " + String(data.joystick_pitch));
  Serial.println("Joystick xAxis: " + String(data.joystick_xAxis) + " | yAxis: " + String(data.joystick_yAxis));
}

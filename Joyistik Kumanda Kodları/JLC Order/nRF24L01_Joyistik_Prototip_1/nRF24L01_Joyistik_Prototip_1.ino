//nRF24 kütüphaneleri
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

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
  delay(1000);
  Serial.println("Verici Başlatıldı...");
}

void loop() {
  // put your main code here, to run repeatedly:
//  boolean transmitting = radio.write(&number, sizeof(number));
//  if(transmitting == true){
//    Serial.println("Transmitting Succesfully");
//  }
//  else{
//    Serial.println("Transmitting Fail!");
//  }
//  number++;
//  delay(1000);
  joystick_control();
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
    Serial.println("," + String(joystick_right_x) + "," + String(joystick_right_y));
    kumanda.xAxis = joystick_right_x;
    kumanda.yAxis = joystick_right_y;
    radio.write(&kumanda, sizeof(Kumanda));
    joystick_right_x_prev = joystick_right_x;
    joystick_right_y_prev = joystick_right_y;
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
  }
  if(!((joystick_right_y < (joystick_right_y_prev + potSensvity)) && (joystick_right_y > (joystick_right_y_prev - potSensvity)))){
    Serial.println("," + String(joystick_right_x) + "," + String(joystick_right_y));
    kumanda.xAxis = joystick_right_x;
    kumanda.yAxis = joystick_right_y;
    radio.write(&kumanda, sizeof(Kumanda));
    joystick_right_x_prev = joystick_right_x;
    joystick_right_y_prev = joystick_right_y;
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
  }
  
  //Serial.println("Joystick Right X: " + String(joystick_right_x));
  //Serial.println("Joystick Right Y: " + String(joystick_right_y));
}

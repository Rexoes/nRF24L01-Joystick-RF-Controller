#include <MPU6050.h>

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

void setup() {
  Serial.begin(9600);

  Serial.println("Initialize MPU6050");
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
}

void loop() {
  directionControl();
}

void directionControl(){
  Vector normAccel = mpu.readNormalizeAccel();

  // Calculate Pitch & Roll
  pitch = -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis*normAccel.YAxis + normAccel.ZAxis*normAccel.ZAxis))*180.0)/M_PI;
  roll = (atan2(normAccel.YAxis, normAccel.ZAxis)*180.0)/M_PI;

  if(!((pitch < (pitch_prev + MPU_Sensvity)) && (pitch > (pitch_prev - MPU_Sensvity)))){
    mpu6050.pitch = pitch;
    mpu6050.roll = roll;
    Serial.println("MPU6050 Pitch: " + String(mpu6050.pitch) + " | Roll: " + String(mpu6050.roll));
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
    Serial.println("MPU6050 Pitch: " + String(mpu6050.pitch) + " | Roll: " + String(mpu6050.roll));
    //radio.write(&kumanda, sizeof(Kumanda));
    pitch_prev = pitch;
    roll_prev = roll;
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
  }
}

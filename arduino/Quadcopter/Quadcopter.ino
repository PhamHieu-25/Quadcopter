#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define MPU6050_ADDR         0x68
#define MPU6050_SMPLRT_DIV   0x19
#define MPU6050_CONFIG       0x1a
#define MPU6050_GYRO_CONFIG  0x1b
#define MPU6050_ACCEL_CONFIG 0x1c
#define MPU6050_WHO_AM_I     0x75
#define MPU6050_PWR_MGMT_1   0x6b
#define MPU6050_TEMP_H       0x41
#define MPU6050_TEMP_L       0x42

int16_t rawAccX, rawAccY, rawAccZ, rawTemp,
        rawGyroX, rawGyroY, rawGyroZ;
float gyroXoffset, gyroYoffset, gyroZoffset;

float temp, accX, accY, accZ, gyroX, gyroY, gyroZ;

float angleGyroX, angleGyroY, angleGyroZ,
      angleAccX, angleAccY, angleAccZ;

float angleX, angleY, angleZ;

float interval;

long preInterval,timelastupdate;

float accCoef = 0.02f, gyroCoef = 0.98f;

long loop_timer = 0;

RF24 radio(7,8);
const byte address[6] = "00002";

int data[4];

int throttle=1000,power1,power2,power3,power4;

Servo motor1,motor2,motor3,motor4;

float roll_setpoint = 0.35 ,pitch_setpoint = -2.00,yaw_setpoint,
      roll_error_now,pitch_error_now,yaw_error_now,
      roll_error_last = 0,pitch_error_last = 0,yaw_error_last = 0,
      roll_error_mem = 0,pitch_error_mem = 0,yaw_error_mem = 0,
      roll_PID = 0,pitch_PID = 0,yaw_PID = 0,
      roll_P = 1.3, roll_I = 0.01, roll_D = 55.5,
      pitch_P = 1.3, pitch_I = 0.01, pitch_D = 55.5,
      yaw_P = 3.0, yaw_I = 0.01, yaw_D = 0,
      roll_control,pitch_control,yaw_control;
int startup = 0; 

void mpu_update();
void writeMPU6050(byte reg, byte data);
void motorWrite(int power1,int power2,int power3,int power4);
void calculate_PID();  

void setup()
{
  Serial.begin(9600);
  
  motor1.attach(3);
  motor2.attach(5);
  motor3.attach(6);
  motor4.attach(9);
  
  motorWrite(1000,1000,1000,1000);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  pinMode(4,OUTPUT);
  digitalWrite(4,0);
  Wire.begin();
  writeMPU6050(MPU6050_SMPLRT_DIV, 0x00);
  writeMPU6050(MPU6050_CONFIG, 0x00);
  writeMPU6050(MPU6050_GYRO_CONFIG, 0x08);
  writeMPU6050(MPU6050_ACCEL_CONFIG, 0x00);
  writeMPU6050(MPU6050_PWR_MGMT_1, 0x01); 
  angleGyroX = 0;
  angleGyroY = 0;
  
  preInterval = millis();
  float x = 0, y = 0, z = 0;
  int16_t rx, ry, rz;
  for(int i = 0; i < 3000; i++){
    
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom((int)MPU6050_ADDR, 6);

    rx = Wire.read() << 8 | Wire.read();
    ry = Wire.read() << 8 | Wire.read();
    rz = Wire.read() << 8 | Wire.read();

    x += ((float)rx) / 65.5;
    y += ((float)ry) / 65.5;
    z += ((float)rz) / 65.5;
  }
  gyroXoffset = x / 3000;
  gyroYoffset = y / 3000;
  gyroZoffset = z / 3000;

  delay(3000);
  for (int i=0;i<200;i++)mpu_update();
  
  digitalWrite(4,1);
  delay(500);
  digitalWrite(4,0);
  delay(500);
  digitalWrite(4,1);
  delay(500);
  digitalWrite(4,0);
}
void loop()
{
  if (radio.available()) 
     {  radio.read(&data,sizeof(data));
        timelastupdate=millis();
      throttle = data[0];
      pitch_control = data[1];
      roll_control = data[2];    
      yaw_control = data[3];
      }
  if(throttle<1000) throttle=1000;
  if(throttle>1500) throttle=1500;

  if(roll_control==0) roll_setpoint = 0.35;
  if(roll_control==1) roll_setpoint =-9.65;
  if(roll_control==-1) roll_setpoint =10.35;
  if(pitch_control==0) pitch_setpoint = -2;
  if(pitch_control==-1) pitch_setpoint=-12;
  if(pitch_control==1) pitch_setpoint=8;
  if(yaw_control==-1) yaw_setpoint-=10;
  if(yaw_control==1) yaw_setpoint+=10;
  
  /*Serial.print(throttle);  
  Serial.print('/');  
  Serial.print(roll_setpoint);
  Serial.print('/');
  Serial.print(pitch_setpoint);
  Serial.print('/');
  Serial.print(yaw_setpoint);
  Serial.println('/');*/
  
  if(millis() - timelastupdate > 4000 && throttle >= 1000)
      throttle-=5;
            
  if(millis() > 5000 && throttle > 1000 )
    startup = 1; 
      else startup = 0;
      
  mpu_update();
  
  if(startup)
  {
    digitalWrite(4,1);
    if(throttle < 1200)
      {
        power1=throttle;
        power2=throttle;
        power3=throttle;
        power4=throttle;
        motorWrite(power1, power2, power3, power4);}
    else
    {
      
      calculate_PID();
      power1 = throttle - yaw_PID - pitch_PID - roll_PID;
      power2 = throttle + yaw_PID - pitch_PID + roll_PID;
      power3 = throttle - yaw_PID + pitch_PID + roll_PID;
      power4 = throttle + yaw_PID + pitch_PID - roll_PID;

      if(power1>2000) power1=2000;if(power1<1100) power1=1100;
      if(power2>2000) power2=2000;if(power2<1100) power2=1100;
      if(power3>2000) power3=2000;if(power3<1100) power3=1100;
      if(power4>2000) power4=2000;if(power4<1100) power4=1100;    
      motorWrite(power1, power2, power3, power4);   
      }  
    }
  
  if(!startup)
    {
      yaw_setpoint=angleZ;
      roll_error_last = 0,pitch_error_last = 0,yaw_error_last = 0,
      roll_error_mem = 0,pitch_error_mem = 0,yaw_error_mem = 0,
      digitalWrite(4,0);
       power1=1000;
       power2=1000;
       power3=1000;
       power4=1000;
       motorWrite(power1, power2, power3, power4);
      }
            
  /*Serial.print(power1);
  Serial.print('/');
    Serial.print(power2);
  Serial.print('/');
    Serial.print(power3);
  Serial.print('/');
    Serial.print(power4);
  Serial.print('/');
      Serial.print(angleX);
  Serial.print('/');
    Serial.print(angleY);
  Serial.print('/');
    Serial.print(angleZ);
  Serial.println('/');*/
  
  while(micros() - loop_timer < 4000);
  loop_timer = micros();      
}

void mpu_update()
{
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom((int)MPU6050_ADDR, 14);

  rawAccX = Wire.read() << 8 | Wire.read();
  rawAccY = Wire.read() << 8 | Wire.read();
  rawAccZ = Wire.read() << 8 | Wire.read();
  rawTemp = Wire.read() << 8 | Wire.read();
  rawGyroX = Wire.read() << 8 | Wire.read();
  rawGyroY = Wire.read() << 8 | Wire.read();
  rawGyroZ = Wire.read() << 8 | Wire.read();

  temp = (rawTemp + 12412.0) / 340.0;

  accX = ((float)rawAccX) / 16384.0;
  accY = ((float)rawAccY) / 16384.0;
  accZ = ((float)rawAccZ) / 16384.0;

  angleAccX = atan2(accY,accZ) * 360 / 2.0 / PI;
  angleAccY = atan2(accX, sqrt(accZ * accZ + accY * accY)) * 360 / -2.0 / PI;

  gyroX = ((float)rawGyroX) / 65.5;
  gyroY = ((float)rawGyroY) / 65.5;
  gyroZ = ((float)rawGyroZ) / 65.5;

  gyroX -= gyroXoffset;
  gyroY -= gyroYoffset;
  gyroZ -= gyroZoffset;

  interval = (millis() - preInterval) * 0.001;

  angleGyroX += gyroX * interval;
  angleGyroY += gyroY * interval;
  angleGyroZ += gyroZ * interval;

  angleX = (gyroCoef * (angleX + gyroX * interval)) + (accCoef * angleAccX);
  angleY = (gyroCoef * (angleY + gyroY * interval)) + (accCoef * angleAccY);
  angleZ = angleGyroZ;
  preInterval = millis(); 
}

void writeMPU6050(byte reg, byte data){
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}  
  
void motorWrite(int power1,int power2,int power3,int power4)
{
  motor1.writeMicroseconds(power1);
  motor2.writeMicroseconds(power2);
  motor3.writeMicroseconds(power3);
  motor4.writeMicroseconds(power4);
}

void calculate_PID()
{
  roll_error_now = roll_setpoint - angleX;
  roll_error_mem += roll_error_now;
  roll_PID = roll_error_now*roll_P+roll_error_mem*roll_I+(roll_error_now-roll_error_last)*roll_D;
  if(roll_PID > 200) roll_PID = 200;
  if(roll_PID < -200) roll_PID = -200;
  roll_error_last = roll_error_now;
  
  pitch_error_now = pitch_setpoint - angleY;
  pitch_error_mem += pitch_error_now;
  pitch_PID = pitch_error_now*pitch_P+pitch_error_mem*pitch_I+(pitch_error_now-pitch_error_last)*pitch_D;
  if(pitch_PID > 200) pitch_PID = 200;
  if(pitch_PID < -200) pitch_PID = -200;
  pitch_error_last = pitch_error_now;

  yaw_error_now = yaw_setpoint - angleZ;
  yaw_error_mem += yaw_error_now;
  yaw_PID = yaw_error_now*yaw_P+yaw_error_mem*yaw_I+(yaw_error_now-yaw_error_last)*yaw_D;
  if(yaw_PID > 100) yaw_PID = 100;
  if(yaw_PID < -100) yaw_PID = -100;
  yaw_error_last = yaw_error_now;
  }  

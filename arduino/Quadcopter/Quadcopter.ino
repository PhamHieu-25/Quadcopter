#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <MPU6050_tockn.h>

MPU6050 mpu6050(Wire);
int startup=0;
float yawReq = 0,
      yawPrev=0, 
      yawErrNow = 0, 
      yawErrIgt = 0, 
      yawErrPrev = 0, 
      yawKp = 0, 
      yawKi = 0, 
      yawKd = 0;

float pitchReq = 0, 
      pitchErrNow = 0, 
      pitchErrIgt = 0, 
      pitchErrPrev = 0, 
      pitchKp = 0, 
      pitchKi = 0, 
      pitchKd = 0;

float rollReq = 0, 
      rollErrNow = 0, 
      rollErrIgt = 0, 
      rollErrPrev = 0, 
      rollKp = 0, 
      rollKi = 0, 
      rollKd = 0;
int throttle = 0,commlastupdate;
int pitch = 0, roll = 0, yaw = 0;
int timer=0;
Servo motor1,motor2,motor3,motor4;
int power1, power2, power3, power4;
RF24 radio(7,8);
const byte address[6] = "00001";
void setup() {
  motor1.attach(3);
  motor2.attach(5);
  motor3.attach(6);
  motor4.attach(9);
  motor1.writeMicroseconds(1000);
  motor2.writeMicroseconds(1000);
  motor3.writeMicroseconds(1000);
  motor4.writeMicroseconds(1000);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void motorWrite(int power1, int power2, int power3, int power4) {
    motor1.writeMicroseconds(power1);
    motor2.writeMicroseconds(power2);
    motor3.writeMicroseconds(power3);
    motor4.writeMicroseconds(power4);
}

void motorCal() {
    power1 = throttle + yaw + pitch + roll;
    power2 = throttle - yaw + pitch - roll;
    power3 = throttle + yaw - pitch - roll;
    power4 = throttle - yaw - pitch + roll;
    
    power1 = constrain(power1, 1000, 2000);
    power2 = constrain(power2, 1000, 2000);
    power3 = constrain(power3, 1000, 2000);
    power4 = constrain(power4, 1000, 2000);
}

void motorUpdate() {
    motorCal();
    motorWrite(power1, power2, power3, power4);
}
void commread(void) {
    if (radio.available()) 
    {
    radio.read(&throttle, sizeof(throttle));
        yawKp = 4;
        yawKi = 0.00525;
        yawKd = 0;
        pitchKp = 2.4;
        pitchKi = 0.0105;
        pitchKd = 62.4;
        rollKp = 2.4;
        rollKi = 0.0105;
        rollKd =  62.4;}
}
void PIDcal()
{
  rollErrNow=rollReq-mpu6050.getAngleX();
  pitchErrNow=pitchReq-mpu6050.getAngleY();
  yawErrNow=yawReq-mpu6050.getAngleZ();

    yawErrIgt += yawErrNow;
    rollErrIgt += rollErrNow;
    pitchErrIgt += pitchErrNow;      

          yaw= yawKp*yawErrNow+yawKd*(yawErrNow-yawErrPrev)+yawKi*yawErrIgt;
          pitch= pitchKp*pitchErrNow+pitchKd*(pitchErrNow-pitchErrPrev)+pitchKi*pitchErrIgt;
          roll= rollKp*rollErrNow+rollKd*(rollErrNow-rollErrPrev)+rollKi*rollErrIgt;
   yawErrPrev=yawErrNow;
     pitchErrPrev=pitchErrNow;
     pitchErrPrev=pitchErrNow;
          
}
void fly()
{
  if (millis() > 15000 && startup==0) {
        yawReq = mpu6050.getAngleZ();
        startup = 1;
    }

  if(startup)
  {
    commread();
    if(throttle>1050)
    {
      PIDcal();
      motorUpdate();}
    else {
      yaw=0;
      pitch=0;
      roll=0;
      motorWrite(throttle,throttle,throttle,throttle);}
    }
    

}
void loop() {
  fly();
   while(micros()-timer<10000){}
  timer=micros();

}

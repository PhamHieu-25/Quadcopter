
#include <Servo.h>
Servo myservo;

#include <MPU6050_tockn.h>
#include <Wire.h>
MPU6050 mpu6050(Wire);

int x, y, z;
int val, valservo, gocservo;

void setup() 
{
  Serial.begin(9600);
  
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  myservo.attach(7);
}

void loop() 
{
  mpu6050.update();
  x = mpu6050.getAngleX();
  y = mpu6050.getAngleY();
  z = mpu6050.getAngleZ();
  Serial.print("X: "); Serial.print(x); Serial.print("      ");
//  Serial.print("Y: "); Serial.print(y); Serial.print("      ");
//  Serial.print("Z: "); Serial.print(z); Serial.print("      ");

  val = map(x,-180,180,0,180);
  Serial.print("Val: "); Serial.print(val); Serial.print("      ");
  
  valservo = val + 5;
  myservo.write(valservo);
  
  gocservo = myservo.read();
  Serial.print("Góc Servo: "); Serial.println(gocservo);
  
  //delay(500);
}


#include <MPU6050_tockn.h>
#include <Wire.h>
MPU6050 mpu6050(Wire);

int x, y, z;
int den = 5;

void setup() 
{
  Serial.begin(9600);
  
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  pinMode(den,OUTPUT);
  digitalWrite(den, LOW);
}

void loop() 
{
  mpu6050.update();
  x = mpu6050.getAngleX();
  y = mpu6050.getAngleY();
  z = mpu6050.getAngleZ();
  Serial.print("X: "); Serial.print(x); Serial.print("      ");
  Serial.print("Y: "); Serial.print(y); Serial.print("      ");
  Serial.print("Z: "); Serial.println(z);

  if (y >= 50 or y <= -50)
  {
    digitalWrite(den, HIGH);
    Serial.println("Đèn sáng");
  }
  else
  {
    digitalWrite(den, LOW);
    Serial.println("Đèn tắt");
  }
  delay(200);
}

#include<Servo.h>
 
Servo ESC; 
 
int val; //Đọc biến trở
int vel; //Độ dài xung gửi cho ESc

void setup()
{
  Serial.begin(9600);

  ESC.attach(7);

}
 
void loop()
{
  val=analogRead(A4);
  Serial.println(val);
  vel=map(val,0,1023,1000,2000);
  ESC.writeMicroseconds(vel);
  
}

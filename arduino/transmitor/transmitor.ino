#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
Servo sv1,sv2; 
RF24 radio(7, 8); //ce,cns
int angle;
int data[4],val,vel,throttle=1000;
const byte address[6] = "00002";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
   sv1.attach(3);
   sv2.attach(4);
   data[0]=1000;
}

void loop() {
  // put your main code here, to run repeatedly:
  /*val=analogRead(A4);
  if(val<10) data[0]+=5;
  else if(val>1000) data[0]-=5;
  data[0]=constrain(data[0],1000,2000);*/
  val=analogRead(A2);
  data[0]=map(val,0,1023,1000,2000);
  val=analogRead(A3);
    if(val<10) data[3]=-1;
  else if(val>1000) data[3]=1;
  else data[3]=0;
   val=analogRead(A1);
    if(val<10) data[2]=-1;
  else if(val>1000) data[2]=1;
  else data[2]=0;
   val=analogRead(A0);
    if(val<10) data[1]=1;
  else if(val>1000) data[1]=-1;
  else data[1]=0;
  radio.write(&data,sizeof(data));
  Serial.print(data[0]);
  Serial.print('/');
  Serial.print(data[1]);
  Serial.print('/');
  Serial.print(data[2]);
  Serial.print('/');
  Serial.println(data[3]);
  delay(10);
}

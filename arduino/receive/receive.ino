#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include<Servo.h>
 
Servo m1,m2,m3,m4;
int vel;
RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  m1.attach(3);
  m2.attach(5);
  m3.attach(6);
  m4.attach(9);
}

void loop() {

  if (radio.available()) {
  
    radio.read(&vel, sizeof(vel));
    m1.writeMicroseconds(vel);
    m2.writeMicroseconds(vel);
    m3.writeMicroseconds(vel);
    m4.writeMicroseconds(vel);
    Serial.println(vel);
  }
  }

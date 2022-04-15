//Global Variable
char c;
String SerialData;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
    while(Serial.available()>0)
      {
          c = Serial.read();
          SerialData += c;
      }

    if(c=='#')//if an End Line Character Received
      {
          if(SerialData=="ON#")
            {
                digitalWrite(13, HIGH);
            }
          else if(SerialData=="OFF#")
            {
                digitalWrite(13, LOW);
            }

          c=0;
          SerialData="";
      }
}

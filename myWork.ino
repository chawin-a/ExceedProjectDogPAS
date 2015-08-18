#include <pt.h>
#include <Servo.h>

#define PT_DELAY(pt, ms, ts) \
  ts = millis(); \
  PT_WAIT_WHILE(pt, millis()-ts < (ms));

#define servoPIN 10
#define sensitivePIN 7
#define trigPIN 2
#define echoPIN 4
#define motorLPINA 6
#define motorLPINB 9
#define motorRPINA 3
#define motorRPINB 5
#define switch1PIN A0
#define switch2PIN A1 
#define SPEED 255

struct pt pt_taskSENSITIVE;
Servo myServo;
String val = "";
int chkOpen = 0;
int chkAll=1;

PT_THREAD(taskSENSITIVE(struct pt* pt))
{
  static uint32_t ts;
  int value;
  int bt1;
  PT_BEGIN(pt);
  while (1)
  {
    value = digitalRead(sensitivePIN);
    chkOpen = 0;
    Serial.println(value);
    if (value == HIGH)
    {
      sendSerial("A");
      while(1){
        switch(val.charAt(0)){
          case '1' : openTheDoor();
                     delay(500);
                     chkOpen = 1;
                     //sendSerial("");
                     break;
          default : closeTheDoor(); break;
        }
        if(chkOpen == 1){
          forward();
          chkSW(switch1PIN);
          //delay(20000);
          wait();
          chkObj();
          delay(1000);
          backward();
          chkSW(switch2PIN);
          closeTheDoor();
          //sendSerial("");
          sendSerial("D");
          delay(1000);
          chkOpen = 0;
          chkAll=0;
          break;
        }
        PT_DELAY(pt, 100, ts);
      }
      PT_DELAY(pt, 200, ts);
    }
    //sendSerial("");
    PT_DELAY(pt, 100, ts); 
  }
  PT_END(pt);
}

void chkSW(int bt){
  int a;
  while(1){
    a = analogRead(bt);
    Serial.println("---------------");
    if(a > 800) a = 1;
    else a = 0;
    Serial.println(a);
    if(!a)break;
    delay(100);
  }
  wait();
}

void chkObj(){
  long duration, distance, sw1;
  //static uint32_t ts;
  while(1){
    sw1 = analogRead(switch1PIN);
    digitalWrite(trigPIN, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPIN, LOW);
    duration = pulseIn(echoPIN, HIGH);
    distance = (duration/2) / 29.1;
    Serial.println(distance);
    if(distance < 6) {
      break;
    }
    delay(3000);
  }
}

void openTheDoor(){
  myServo.write(170);
  delay(1000);
}

void closeTheDoor(){
  myServo.write(70);
  delay(1000);
}

void serialEvent() {
  if (Serial1.available() > 0) {
    val = Serial1.readStringUntil('\r');
    /*forward();
    delay(1000);
    backward();
    delay(1000);
    wait();
    delay(1000);*/
    Serial.print("value Recieve : ");
    Serial.println(val);
    Serial.flush();
  }
}

void sendSerial(String text) {
  Serial1.print(text);
  Serial1.print('\r');
  Serial.println("Send.");
}

void forward(){
  Serial.println("Forward");
  analogWrite(motorLPINA, SPEED);
  analogWrite(motorRPINA, SPEED);
  /*for(int motorValue = 0 ; motorValue <= SPEED; motorValue +=5){
    analogWrite(motorRPINA, motorValue); 
    //analogWrite(motorRPINB, 255 - motorValue);
    analogWrite(motorLPINA, motorValue); 
    //analogWrite(motorLPINB, 255 - motorValue); 
    delay(30);      
  }*/
}

void backward(){
  Serial.println("Backward");
  analogWrite(motorLPINB, SPEED);
  analogWrite(motorRPINB, SPEED);
  /*for(int motorValue = 0 ; motorValue <= SPEED; motorValue +=5){
    //analogWrite(motorRPINA, motorValue); 
    analogWrite(motorRPINB, motorValue);
    //analogWrite(motorLPINA, motorValue); 
    analogWrite(motorLPINB, motorValue); 
    delay(30);      
  }*/
}

void wait(){
  Serial.println("Stop");
  analogWrite(motorLPINA, 0);
  analogWrite(motorRPINA, 0);
  analogWrite(motorLPINB, 0);
  analogWrite(motorRPINB, 0);
}

void setup() 
{
  Serial.begin (9600);
  Serial1.begin(115200);
  pinMode(trigPIN, OUTPUT);
  pinMode(echoPIN, INPUT);
  pinMode(sensitivePIN, INPUT);
  myServo.attach(servoPIN);
  myServo.write(70);
  pinMode(motorLPINA, OUTPUT);
  pinMode(motorLPINB, OUTPUT);
  pinMode(motorRPINA, OUTPUT);
  pinMode(motorRPINB, OUTPUT);
  analogWrite(motorRPINA, 55);
  analogWrite(motorRPINB, 55);
  analogWrite(motorLPINA, 0);
  analogWrite(motorLPINB, 0);
  PT_INIT(&pt_taskSENSITIVE);
}

void loop() {
  taskSENSITIVE(&pt_taskSENSITIVE);
  serialEvent();
}

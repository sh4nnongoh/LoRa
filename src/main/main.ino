#include <SoftwareSerial.h>

#define LED_0 13
#define M0 12
#define M1 11
#define SOFT_TX 10
#define SOFT_RX 9
#define AUX 8

SoftwareSerial mySerial(SOFT_RX,SOFT_TX,true); // RX, TX

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_0, OUTPUT);
  
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0,LOW);
  digitalWrite(M1,LOW);
    
  pinMode(SOFT_RX, INPUT);
  pinMode(SOFT_TX, OUTPUT);

  pinMode(AUX, INPUT);
   
  Serial.begin(9600);
  while(!Serial){
    ;
  }

  Serial.println("Goodnight moon!");

  mySerial.begin(9600);
  mySerial.println("123?");

  // Setup Delay
  delay(620);
}

bool Read(int length=8){
  if (mySerial.available()>0){
    Serial.println("\nAttempting to read...");

    // Wait till msg is stored in LoRa module
    while(!digitalRead(AUX)){
      digitalWrite(LED_0,LOW);
    }
    digitalWrite(LED_0,HIGH); 
    
    // Only start reading at $; 36 == $
    while(mySerial.read()!=36);

    // Obtain msg length
    byte b1 = mySerial.read();
    delay(1);
    byte b2 = mySerial.read();
    delay(1);
    unsigned int length = ((unsigned int)b1 << 8) | (unsigned int)b2;
    int size = length*2;

    // Read msg
    byte buffer[size];
    mySerial.readBytesUntil(char(35),buffer,size); // 35 == #
    for(int i=0;i<size/2;i++){
      buffer[i] = buffer[(i*2)+1];
      //Serial.print(buffer[i],HEX);
    }
    Serial.println();
    buffer[length] = '\0';
    String msg = (char*)buffer;
    Serial.println(msg);

    return true;
  }
  // Must be 40ms
  delay(40);
  return false;
}

bool Write(byte msg[], int msgSize){

  digitalWrite(LED_0,HIGH); 
  
  Serial.println("\nAttempting to send...");
  byte start = 0x24; // Starting Character
  mySerial.write(char(highByte(start)));
  mySerial.write(char(lowByte(start)));

  if(msgSize>250) return false;
  mySerial.write(char(highByte(msgSize)));
  mySerial.write(char(lowByte(msgSize)));
  
  for(int i=0;i<msgSize;i++){
    mySerial.write(char(highByte(msg[i])));
    mySerial.write(char(lowByte(msg[i])));
  }
  byte endline = 0x25; // Ending Character
  mySerial.write(char(highByte(endline)));
  mySerial.write(char(lowByte(endline)));
  
  //delay before sending
  delay(5);
  //ensure all is sent over
  while(!digitalRead(AUX));
  Serial.println("Sent...");
  
  digitalWrite(LED_0,LOW); 
  return true;
}

bool ChangeMode(int mode=0){
  int m0 = 0;
  int m1 = 0;

  switch(mode){
    case 0:
      m0 = 0;
      m1 = 0;
      break;
    case 1:
      m0 = 1;
      m1 = 0;
      break;
    case 2:
      m0 = 0;
      m1 = 1;
      break;
    case 3:
      m0 = 1;
      m1 = 1;
      break;
    default:
      return false;
  }
  if(digitalRead(AUX)){ 
    digitalWrite(M0,m0);
    digitalWrite(M1,m1);
    // Need to be 4ms
    delay(4);
  }
  while(!digitalRead(AUX));
  return true;
}

int Voltage(){
  //Serial.println("Printing Voltage,");
  byte msg[3] = {0xC5, 0xC5, 0xC5};
  Write(msg,3);
  Read();
  return 0;
}

byte Configuration(){
  //Serial.println("Printing Configuration,");
  byte msg[3] = {0xC1, 0xC1, 0xC1};
  Write(msg,3);
  Read();
  return 0;
}

bool flag = true;
void loop() {

  if(flag){
    //ChangeMode(3);
    //Configuration();
    //Voltage();
  }
  //Read();
  
  
  if(flag){
    byte msgArray[] = {};
    String msgString = "abcdefghijk";
    msgString.getBytes(msgArray,msgString.length()+1);
    Write(msgArray,msgString.length());
  }
  if(Read()){
    byte msgArray[] = {};
    String msgString = "abcdefghijk";
    msgString.getBytes(msgArray,msgString.length()+1);
    Write(msgArray,msgString.length());
  }
  
  /*
  if(Read()){
    byte msgArray[] = {};
    String msgString = "ack";
    msgString.getBytes(msgArray,msgString.length()+1);
    Write(msgArray,msgString.length());
  }
  */
  delay(1000);
  flag = false;
}



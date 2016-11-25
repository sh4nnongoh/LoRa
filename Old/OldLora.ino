/*
 Name:    LoRa.ino
 Created: 9/4/2016 10:28:40 AM
 Author:  Shannon
*/
const int sensorPin = A0;
const float baselineTemp = 20.0;
#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
#include <SoftwareSerial.h>

// Set debug mode here
#define DEBUG 1
// Set the module mode here; 1 - Transmitter / 0 - Receiver
#define INIT_MODE 0

#define LED_0 13
#define M0 12
#define M1 11
#define SOFT_TX 10
#define SOFT_RX 9
#define AUX 8

SoftwareSerial mySerial(SOFT_RX, SOFT_TX, true); // RX, TX

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  pinMode(LED_0, OUTPUT);

  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);

  pinMode(SOFT_RX, INPUT);
  pinMode(SOFT_TX, OUTPUT);

  pinMode(AUX, INPUT);

  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  if(DEBUG)
    Serial.println("Goodnight moon!");

  mySerial.begin(9600);
 
  // Setup Delay
  delay(620);
}

String Read() {
  int read_count=0;
  String msg = "";
  if (mySerial.available() > 0) {
    if(DEBUG)
      Serial.println("\nAttempting to read...");

    //
    // Wait till msg is stored in LoRa module
    //
    while (!digitalRead(AUX)) {
      digitalWrite(LED_0, LOW);
    }
    digitalWrite(LED_0, HIGH);
    
    //
    // Only start reading at $; 36 == $
    //
    while (mySerial.read() != 36){
      read_count++;
      // Simple watchdog; to prevent deadlock.
      if(read_count==9999){
        Serial.println("Read Fail.");
        return "";
      }
    }

    //
    // Obtain msg length
    //
    byte b1 = mySerial.read();
    byte b2 = mySerial.read();
    unsigned int length = ((unsigned int)b1 << 8) | (unsigned int)b2;
    int size = length * 2;

    //
    // Read msg
    //
    byte buffer[size];
    mySerial.readBytesUntil(char(35), buffer, size); // 35 == #
    // Apparently there are 0s slotted between each char.
    for (int i = 0; i < size / 2; i++) {
      buffer[i] = buffer[(i * 2) + 1];
      if(DEBUG)
        Serial.print(buffer[i],HEX);
    }
    if(DEBUG)
      Serial.println();
    buffer[length] = '\0';
    msg = (char*)buffer;
    Serial.println(msg);

    return msg;
  }

  return "";
}

bool Write(unsigned char msg[], int msgSize) {

  digitalWrite(LED_0, HIGH);
  if(DEBUG)
    Serial.println("\nAttempting to send...");
  byte start = 0x24; // Starting Character
  mySerial.write(char(highByte(start)));
  mySerial.write(char(lowByte(start)));

  if (msgSize > 250) return false;
  mySerial.write(char(highByte(msgSize)));
  mySerial.write(char(lowByte(msgSize)));

  for (int i = 0; i < msgSize; i++) {
    mySerial.write(char(highByte(msg[i])));
    mySerial.write(char(lowByte(msg[i])));
  }
  byte endline = 0x25; // Ending Character
  mySerial.write(char(highByte(endline)));
  mySerial.write(char(lowByte(endline)));

  //delay before sending
  delay(5);
  //ensure all is sent over
  while (!digitalRead(AUX));
  if(DEBUG)
    Serial.println("Sent...");

  digitalWrite(LED_0, LOW);
  return true;
}

//
// Global Variables
//
bool flag = true;
int count = 0;

void loop() {

  unsigned char msgArray[256];
  String msgString;
  String msgStrings[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
  static int msgStringsPtr = 0;
  String temp;

  const int sensorPin = A0;
  const float baselineTemp = 20.0;
  int sensorVal;
  float voltage;
  float temperature;
    
  switch (INIT_MODE) {
  case 0:
    // 
    // Section of code for reading
    //
    // Only sends "ack" when a specific string is received
    //if(Read()=="abcdefghijk"){
    temp = Read();
    //if(temp=="0"||temp=="1"||temp=="2"||temp=="3"||temp=="4"||temp=="5"||temp=="6"||temp=="7"||temp=="8"||temp=="9"){
    if(temp!=""){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(temp);
      msgString = "ack";
      msgString.getBytes(msgArray,msgString.length()+1);
      Write(msgArray,msgString.length());
    }
    else{
      //lcd.print("fail");
    }
    break;
  case 1:
    //
    // Section of code for transmitting
    //
    // The first packet will be transmitted without checks for "ack"
    // Subsequent packets will check for "ack" before transmiting again.

    sensorVal = analogRead(sensorPin);
    voltage = (sensorVal/1024.0) * 5.0;
    temperature = (voltage - .5) * 100;
    if(DEBUG){
      Serial.print("Sensor Value: ");
      Serial.print(sensorVal);
      Serial.print(", Volts: ");
      Serial.print(voltage);
      Serial.print(", degree C: ");
      Serial.println(temperature);
    }
    lcd.print("Degree C: "); 
    lcd.print(temperature,5);

    if (flag || Read()=="ack") {
      msgString = msgStrings[msgStringsPtr] + " Degree C: " + temperature;
      //msgString = msgStrings[msgStringsPtr];
      msgStringsPtr = (msgStringsPtr + 1) % 10;
      msgString.getBytes(msgArray, msgString.length() + 1);
      Write(msgArray, msgString.length());
      flag = false;
    }
    else{
      count++;
      // If no "ack" received after 5 counts, resend packet.
      if(count==2){
        count = 0;
        flag = true;
      }
    }
    break;
  default: break;
  }

  delay(1000);
  //lcd.clear();
  //lcd.setCursor(0,0);
}

/*
bool ChangeMode(int mode = 0) {
  int m0 = 0;
  int m1 = 0;

  switch (mode) {
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
  if (digitalRead(AUX)) {
    digitalWrite(M0, m0);
    digitalWrite(M1, m1);
    // Need to be 4ms
    delay(4);
  }
  while (!digitalRead(AUX));
  return true;
}

int Voltage() {
  //Serial.println("Printing Voltage,");
  byte msg[3] = { 0xC5, 0xC5, 0xC5 };
  Write(msg, 3);
  Read();
  return 0;
}

byte Configuration() {
  //Serial.println("Printing Configuration,");
  byte msg[3] = { 0xC1, 0xC1, 0xC1 };
  Write(msg, 3);
  Read();
  return 0;
}
*/

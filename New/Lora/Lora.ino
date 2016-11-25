/*
 Name:    Lora.ino
 Created: 11/5/2016 2:48:03 PM
 Author:  Shannon
*/

#include <Lora.h>
#include <LiquidCrystal.h>

// Set debug mode here
#define DEBUG 1
// Set the module mode here; 1 - Transmitter / 0 - Receiver
#define INIT_MODE 1

// Declare Global Variables
// *** MUST DECLARE THE RIGHT PINS ***
Lora lora(12,11,10,9,8,13,9600);
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

bool flag;
int count;
String temp;
int msgStringsPtr = 0;
char ack[] = "ack\0";

void setup() {

  lcd.begin(16,2);

  Serial.begin(9600);
  Serial.println("Initializing Lora");
  lora.initialize();
  flag = true;
  count = 0;

  // Setup Delay
  delay(1000);
}

void loop() {
    
  switch (INIT_MODE) {
  case 0:
    // 
    // Section of code for reading
    //
    // Only sends "ack" when a specific string is received

    // *** Different ways to declare input.***
    // Method 1: Use a Byte Array
    int NumOfMsg = 20;
    byte msgStrings[20] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49};
    // Method 2: Use a Char Array
    //int NumOfMsg = 10;
    //char msgStrings[10] = "0123456789";

    // If there is data received, write ACK.
    temp = lora.read();
    if(temp!=""){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(temp);
      lora.write(ack, 3);
    }
    break;
  case 1:
    //
    // Section of code for transmitting
    //
    // The first packet will be transmitted without checks for "ack"
    // Subsequent packets will check for "ack" before transmiting again.
    temp = lora.read();
    if(DEBUG){
      Serial.println(temp);
    }
    // *** The ACKNOWLEDGE CHECK IS STILL BUGGY ***
    // *** So disabled for now ***
    //if (flag || temp.compareTo("ack")>0) {
    if (flag){
      msgStringsPtr = (msgStringsPtr + 1) % NumOfMsg;
      lora.write(msgStrings+msgStringsPtr, 1);
      flag = false;
    }
    else{
      count++;
      // If no "ack" received after X counts, send next packet.
      if(count==2){
        count = 0;
        flag = true;
      }
    }
    break;
  default: break;
  }

  delay(1000);
}

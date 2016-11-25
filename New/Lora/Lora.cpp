/*
 Name:    Lora.cpp
 Created: 11/5/2016 2:48:03 PM
 Author:  Shannon
 Editor:  http://www.visualmicro.com
*/

#include "Lora.h"

#define DEBUG 1

Lora::Lora() {
  _m0 = 12;
  _m1 = 11;
  _tx = 9;
  _rx = 8;
  _aux = 7;
  _debugLED = 13;
  _baudRate = 9600;
}

Lora::Lora(unsigned char m0, unsigned char m1, unsigned char tx, unsigned char rx, unsigned char aux, unsigned char debugLED, long baudRate) {
  _m0 = m0;
  _m1 = m1;
  _tx = tx;
  _rx = rx;
  _aux = aux;
  _debugLED = debugLED;
  _baudRate = baudRate;
}

void Lora::initialize() {

  pinMode(_debugLED, OUTPUT);

  pinMode(_m0, OUTPUT);
  pinMode(_m1, OUTPUT);
  digitalWrite(_m0, LOW);
  digitalWrite(_m1, LOW);

  pinMode(_rx, INPUT);
  pinMode(_tx, OUTPUT);

  pinMode(_aux, INPUT);

  _serial = new SoftwareSerial(_rx, _tx, true);
  _serial->begin(_baudRate);

  // Setup Delay
  delay(1000);
}

Lora::~Lora() {
}

String Lora::read() {

  uint8_t read_count = 0;
  uint8_t temp = 0;
  String msg = "";

  if (_serial->available() >= sizeof(256)) {

    if (DEBUG) {
      Serial.println("\nAttempting to read...");
    }

    //
    // Wait till msg is stored in LoRa module
    //
    while (!digitalRead(_aux)) {
      digitalWrite(_debugLED, LOW);
    }
    digitalWrite(_debugLED, HIGH);

    //
    // Only start reading at 0x02
    //
    temp = _serial->read();
    while (temp != 0x02) {
      read_count++;
      // Simple watchdog; to prevent deadlock.
      if (read_count == 9999) {
        Serial.println("Read Fail.");
        return "";
      }
      temp = _serial->read();
    }

    //
    // Obtain msg length
    //
    int size = (_serial->read() | 0) + 1;

    if (DEBUG) {
      Serial.print("size: ");
      Serial.print(size);
      Serial.println();
    }

    //
    // Read msg; ends with 0x03
    //
    byte * buffer = new byte[size];
    _serial->readBytesUntil(char(0x03), buffer, size); 
    buffer[size] = '\0';

    msg = (char*)buffer;
    if (DEBUG) {
      Serial.println(msg);
    }

    return msg;
  }

  return "";
}



bool Lora::write(uint8_t msg[], int msgLen) {

  if (msgLen > 253) return false;

  digitalWrite(_debugLED, HIGH);
  if (DEBUG){
    Serial.print("\nAttempting to send: ");
    Serial.write(msg,msgLen);
    Serial.println();
  }

  byte nul = 0x00; // Initiate
  _serial->write(nul);

  byte start = 0x02; // Starting Character
  _serial->write(start);

  _serial->write(byte(msgLen));

  _serial->write(msg,msgLen);

  byte end = 0x03; // Ending Character
  _serial->write(end);

  delay(5);
  //ensure all is sent over
  while (!digitalRead(_aux));
  if (DEBUG) {
    Serial.println("Sent...");
  }

  digitalWrite(_debugLED, LOW);
  return true;
}

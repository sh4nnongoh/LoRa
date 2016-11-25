/*
 Name:    Lora.h
 Created: 11/5/2016 2:48:03 PM
 Author:  Shannon
 Editor:  http://www.visualmicro.com
*/

#ifndef _Lora_h
#define _Lora_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif

#include <SoftwareSerial.h>
//#include <AESLib.h> 



class Lora {

public:
  Lora();
  Lora(unsigned char m0, unsigned char m1, unsigned char tx, unsigned char rx, unsigned char aux, unsigned char debugLED, long baudRate);
  ~Lora();

  String read();
  bool write(uint8_t msg[], int msgLen);

  void initialize();

private:

  SoftwareSerial* _serial;

  // Pins
  unsigned char
    _m0,
    _m1,
    _tx,
    _rx,
    _aux,
    _debugLED;

  long _baudRate;

  //uint8_t key[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  
};


#endif


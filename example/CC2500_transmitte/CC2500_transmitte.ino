/**
 * CC2500 Sample transceiver code
 * Ported from https://github.com/yasiralijaved/Arduino-CC2500-Library
 * Written by Zohan SonZohan@gmail.com
 *
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCLK/SCK -> 13
 * CSN/SS - > 10
 */
#include "cc2500.h"

//cc2500 radio;
cc2500 radio(10); // CS

void setup(){
  Serial.begin(115200);
  Serial.println("Initializing Wireless..");
  if (radio.begin() == false) { 
    Serial.println("Device Not Installed");
    while(1);
  }
}

void loop(){
  long sendInterval = 1000; // in milliseconds
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > sendInterval) {
    previousMillis = currentMillis;   
    uint8_t txBuf[64];
    int txLen = 32;
    for (int i=0;i<txLen;i++) txBuf[i] = i;
    radio.sendPacket(txBuf, txLen);
  }
  //radio.listenForPacket();
}

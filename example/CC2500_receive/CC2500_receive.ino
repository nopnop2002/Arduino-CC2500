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
#if 0
  long sendInterval = 1000; // in milliseconds
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > sendInterval) {
    previousMillis = currentMillis;
    uint8_t txBuf[64];
    strcpy((char *)txBuf, "hello");
    radio.sendPacket(txBuf, 5);
  }
#endif

  uint8_t rxBuf[64] = {0};
  uint8_t rssi;
  uint8_t lqi;
  int PacketLength = radio.listenForPacket(rxBuf, 64, &rssi, &lqi);
  if (PacketLength) {
    Serial.print("PacketLength: ");
    Serial.println(PacketLength);
    for (int i=0;i<PacketLength;i++) {
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    for (int i=0;i<PacketLength;i++) {
      char c = rxBuf[i];
      if (c > 0x20 && c < 0xFF) {
        Serial.print(c);
      } else {
        Serial.print(" ");
      }
    }
    Serial.println();

    int dbm;
    if (rssi < 0x7F) {
      dbm = (rssi/2) - 72;
    } else {
      dbm = abs(rssi-256);
      dbm = (dbm/2) - 72;
    }

    Serial.print("RSSI(Raw): 0x");
    Serial.print(rssi, HEX);
    Serial.print(" RSSI(dBm): ");
    Serial.print(dbm);
    Serial.print(" LQI: 0x");
    Serial.println(lqi, HEX);
  }
  
}

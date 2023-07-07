# Arduino-CC2500
Arduino library for TEXAS INSTRUMENTS CC2500.

I made it with reference to [this](https://github.com/Zohan/ArduinoCC2500Demo).   

![CC2500-2400MHz-2](https://user-images.githubusercontent.com/6020549/163664994-4b84de76-78fe-46d0-9658-93f8b3c04b19.JPG)


# Changes from the original

- Changed legacy SPI communication to SPI communication with transactions.   

- Added CS pin parameters to the constructor.   

- Added getChipPart function.   

- Added getChipVersion function.   

- Added resetDevice function.   

- Changed some initial parameters.   

- Changed example code.   


# Wireing

|CC2500||UNO|MEGA|ESP8266|
|:-:|:-:|:-:|:-:|:-:|
|GND|--|GND|GND|GND|
|CSN|--|D10(*2)|D10(*2)|IO15|
|MISO|--|D12|D50|IO12|
|SCLK|--|D13(*2)|D52(*2)|IO14|
|MOSI|--|D11(*2)|D51(*2)|IO13|
|VCC|--|3.3V(*1)|3.3V|3.3V|

(*1)   
UNO's 3.3V output can only supply 50mA.   
In addition, the output current capacity of UNO-compatible devices is smaller than that of official products.   
__So this module may not work normally when supplied from the on-board 3v3.__   

(*2)   
CC2500 is not 5V tolerant.   
You need level shift from 5V to 3.3V.   
I used [this](https://www.ti.com/lit/ds/symlink/txs0108e.pdf?ts=1647593549503) for a level shift.   


# Communication with another library   
Basically, communication between applications with different register values is not guaranteed.   
These settings may differ.   
- Base frequency and channel spacing frequency
- Packet format configuration
- Manchester encoding configuration
- Wireless modulation method
- The data rate used when transmitting
- The data rate expected in receive



#include "cc2500.h"

cc2500::cc2500() {
}

cc2500::cc2500(int8_t chipSelect) {
	_chipSelect = chipSelect;
}

bool cc2500::begin(uint8_t channel) {
	SPI.begin();
	//remove regacy mode
	//SPI.setClockDivider(SPI_CLOCK_DIV2);
	//SPI.setBitOrder(MSBFIRST);
	//SPI.setDataMode(SPI_MODE0);

	Serial.print("_chipSelect=");
	Serial.println(_chipSelect);
	digitalWrite(_chipSelect,HIGH);
	pinMode(_chipSelect,OUTPUT);

	resetDevice();
	Serial.print("channel=");
	Serial.println(channel);
	init(channel);

	uint8_t ChipPart = getChipPart();
	uint8_t ChipVersion = getChipVersion();
	Serial.print("ChipPart=0x");
	Serial.println(ChipPart, HEX);
	Serial.print("ChipVersion=0x");
	Serial.println(ChipVersion, HEX);
	if (ChipPart != 0x80 || ChipVersion != 0x03) {
		Serial.println("CC2500 Not Installed");
		return false;
	}
	Serial.println("CC2500 Installed");
	return true;
}

void cc2500::init(uint8_t channel) {
	WriteRegister(REG_IOCFG2,0x06);
	WriteRegister(REG_IOCFG0,0x01);
	WriteRegister(REG_IOCFG1,0x06);

	//WriteRegister(REG_FIFOTHR, 0x02);
	WriteRegister(REG_FIFOTHR, VAL_FIFOTHR);
	WriteRegister(REG_SYNC1,VAL_SYNC1);
	WriteRegister(REG_SYNC0,VAL_SYNC0);
	WriteRegister(REG_PKTLEN,VAL_PKTLEN);
	//WriteRegister(REG_PKTCTRL1,0x8C);
	WriteRegister(REG_PKTCTRL1,VAL_PKTCTRL1);
	//WriteRegister(REG_PKTCTRL0, 0x0D);
	WriteRegister(REG_PKTCTRL0, 0x44); // Changed to Fixed packet length mode.

	WriteRegister(REG_ADDR,VAL_ADDR);
	//WriteRegister(REG_CHANNR,VAL_CHANNR);
	WriteRegister(REG_CHANNR,channel);
	WriteRegister(REG_FSCTRL1,VAL_FSCTRL1);
	WriteRegister(REG_FSCTRL0,VAL_FSCTRL0);
	WriteRegister(REG_FREQ2,VAL_FREQ2);
	WriteRegister(REG_FREQ1,VAL_FREQ1);
	WriteRegister(REG_FREQ0,VAL_FREQ0);
	WriteRegister(REG_MDMCFG4,VAL_MDMCFG4);
	WriteRegister(REG_MDMCFG3,VAL_MDMCFG3);
	WriteRegister(REG_MDMCFG2,VAL_MDMCFG2);
	WriteRegister(REG_MDMCFG1,VAL_MDMCFG1);
	//WriteRegister(REG_MDMCFG1,0xA2); // Enable Forward Error Correction (FEC) with interleaving for packet payload
	WriteRegister(REG_MDMCFG0,VAL_MDMCFG0);
	WriteRegister(REG_DEVIATN,VAL_DEVIATN);
	WriteRegister(REG_MCSM2,VAL_MCSM2);
	WriteRegister(REG_MCSM1,VAL_MCSM1);
	WriteRegister(REG_MCSM0,VAL_MCSM0);
	WriteRegister(REG_FOCCFG,VAL_FOCCFG);

	WriteRegister(REG_BSCFG,VAL_BSCFG);
	WriteRegister(REG_AGCCTRL2,VAL_AGCCTRL2);
	WriteRegister(REG_AGCCTRL1,VAL_AGCCTRL1);
	WriteRegister(REG_AGCCTRL0,VAL_AGCCTRL0);
	WriteRegister(REG_WOREVT1,VAL_WOREVT1);
	WriteRegister(REG_WOREVT0,VAL_WOREVT0);
	WriteRegister(REG_WORCTRL,VAL_WORCTRL);
	WriteRegister(REG_FREND1,VAL_FREND1);
	WriteRegister(REG_FREND0,VAL_FREND0);
	WriteRegister(REG_FSCAL3,VAL_FSCAL3);
	WriteRegister(REG_FSCAL2,VAL_FSCAL2);
	WriteRegister(REG_FSCAL1,VAL_FSCAL1);
	WriteRegister(REG_FSCAL0,VAL_FSCAL0);
	WriteRegister(REG_RCCTRL1,VAL_RCCTRL1);
	WriteRegister(REG_RCCTRL0,VAL_RCCTRL0);
	WriteRegister(REG_FSTEST,VAL_FSTEST);
	WriteRegister(REG_PTEST,VAL_PTEST);
	WriteRegister(REG_AGCTEST,VAL_AGCTEST);
	WriteRegister(REG_TEST2,VAL_TEST2);
	WriteRegister(REG_TEST1,VAL_TEST1);
	WriteRegister(REG_TEST0,VAL_TEST0);
}

void cc2500::resetDevice(void) {
	SendStrobe(CC2500_CMD_SRES);
}



int cc2500::listenForPacket(uint8_t *buf, int8_t blen, uint8_t *rssi, uint8_t *lqi) {
	WriteRegister(REG_IOCFG1,0x06);

	int PacketLength = 0;
	SendStrobe(CC2500_CMD_SRX);
	// Switch MISO to output if a packet has been received or not
	WriteRegister(REG_IOCFG1,0x01);
	delay(20);
	if (digitalRead(MISO)) {
		PacketLength = ReadRegister(CC2500_RX_FIFO) - 1;
		//Serial.println("Packet Received!");
		//Serial.print("Packet Length: ");
		//Serial.println(PacketLength, DEC);
		if (PacketLength > blen) PacketLength = blen;
		//Serial.print("Data: ");
		for(int i = 0; i < PacketLength; i++){
			buf[i] = ReadRegister(CC2500_RX_FIFO);
			//Serial.print(buf[i], HEX);
			//Serial.print(" ");
		}
		//Serial.println();

		uint8_t pktctrl1 = ReadRegister(REG_PKTCTRL1);
		//Serial.print("pktctrl1: ");
		//Serial.println(pktctrl1, HEX);
		if (( pktctrl1 & 0x04) == 0x04) {
			// APPEND_STATUS
			// When enabled, two status bytes will be appended to the payload of the packet.
			// The status bytes contain RSSI and LQI values, as well as the CRC OK flag.
			*rssi = ReadRegister(CC2500_RX_FIFO);
			*lqi = ReadRegister(CC2500_RX_FIFO);
			//Serial.print("RSSI: ");
			//Serial.println(*rssi);
			//Serial.print("LQI: 0x");
			//Serial.println(*lqi, HEX);
		}
		    
		// Make sure that the radio is in IDLE state before flushing the FIFO
		// (Unless RXOFF_MODE has been changed, the radio should be in IDLE state at this point) 
		SendStrobe(CC2500_CMD_SIDLE);
		// Flush RX FIFO
		SendStrobe(CC2500_CMD_SFRX);
	} else {

	}
	return PacketLength;
}

void cc2500::sendPacket(uint8_t *buf, int blen) {
	WriteRegister(REG_IOCFG1,0x06);
	// Make sure that the radio is in IDLE state before flushing the FIFO
	SendStrobe(CC2500_CMD_SIDLE);
	// Flush TX FIFO
	SendStrobe(CC2500_CMD_SFTX);
	// prepare Packet
	int length = blen + 1;
	uint8_t packet[length];
	// First Byte = Length Of Packet
	packet[0] = length;
	for (int i=0;i<blen;i++) {
		packet[i+1] = buf[i];
	}

	// SIDLE: exit RX/TX
	SendStrobe(CC2500_CMD_SIDLE);

	//Serial.println("Transmitting ");
	for(int i = 0; i < length; i++)
	{    
		WriteRegister(CC2500_TX_FIFO,packet[i]);
	}
	// STX: enable TX
	SendStrobe(CC2500_CMD_STX);
	// Wait for GDO0 to be set -> sync transmitted
	unsigned long previousTXTimeoutMillis = millis();
	while(1) {
		if (digitalRead(MISO)) break;
		//Serial.println("wait for HIGH");
	}
	unsigned long elaspedTXTimeoutMillis = millis() - previousTXTimeoutMillis;
	//Serial.print("elaspedTXTimeoutMillis=");
	//Serial.println(elaspedTXTimeoutMillis);
	   
	// Wait for GDO0 to be cleared -> end of packet
	previousTXTimeoutMillis = millis();
	while(1) {
		if (!digitalRead(MISO)) break;
		//Serial.println("wait for LOW");
	}
	elaspedTXTimeoutMillis = millis() - previousTXTimeoutMillis;
	//Serial.print("elaspedTXTimeoutMillis=");
	//Serial.println(elaspedTXTimeoutMillis);

	//Serial.println("Finished sending");
	SendStrobe(CC2500_CMD_SIDLE);
}


void cc2500::WriteRegister(char addr, char value) {
	digitalWrite(_chipSelect,LOW);
	SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
	while (digitalRead(MISO) == HIGH) {
	};

	SPI.transfer(addr|CC2500_WRITE_SINGLE);
	//delay(10);
	SPI.transfer(value);
	digitalWrite(_chipSelect,HIGH);
	SPI.endTransaction();
}

uint8_t cc2500::ReadRegister(char addr) {
	digitalWrite(_chipSelect,LOW);
	SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
	while (digitalRead(MISO) == HIGH) {
	};

	SPI.transfer(addr|CC2500_READ_SINGLE);
	//delay(10);
	uint8_t result = SPI.transfer(0);
	digitalWrite(_chipSelect,HIGH);
	SPI.endTransaction();
	return result;  
}

uint8_t cc2500::ReadStatus(uint8_t addr) {
	digitalWrite(_chipSelect,LOW);
	SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
	while (digitalRead(MISO) == HIGH) {
	};

	SPI.transfer(addr|CC2500_READ_STATUS);
	//delay(10);
	uint8_t result = SPI.transfer(0);
	digitalWrite(_chipSelect,HIGH);
	SPI.endTransaction();
	return result;  
}


uint8_t cc2500::SendStrobe(char strobe) {
	digitalWrite(_chipSelect,LOW);
	SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
	while (digitalRead(MISO) == HIGH) {
	};

	uint8_t result =  SPI.transfer(strobe);
	digitalWrite(_chipSelect,HIGH);
	SPI.endTransaction();
	//delay(10);
	return result;
}

uint8_t cc2500::getChipPart(void) {
	return ReadStatus(REG_PARTNUM);
}

uint8_t cc2500::getChipVersion(void) {
	return ReadStatus(REG_VERSION);
}

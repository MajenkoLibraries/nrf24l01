#include <nrf24l01.h>

DSPI0 spi;

// SPI port DSPI0, NCS is pin 10, CE is pin 9 and the interrupt is INT1
nrf24l01 rf(spi, 10, 9, 1);

void setup() {
	spi.begin();
    // Join mesh number 1, with my address 10,0,0,1 on channel 0.
	rf.begin(1, 10,0,0,1, 0);
}

void loop() {
    // Packets are 32 bytes in size.
	uint8_t buffer[32];

    // We want to talk to 10,0,0,2 on mesh 1.
	uint8_t dest[5] = {1, 10,0,0,2 };

	buffer[0] = 1;
	rf.queuePacket(dest, buffer);
	delay(100);
	buffer[0] = 0;
	rf.queuePacket(dest, buffer);
	delay(1000);	
}

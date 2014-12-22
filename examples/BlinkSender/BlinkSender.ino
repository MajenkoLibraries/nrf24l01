#include <nrf24l01.h>

DSPI0 spi;

nrf24l01 rf(spi, 10, 9, 1);

void setup() {
	spi.begin();
	rf.begin(1, 10,0,0,1, 0);
}

void loop() {
	uint8_t buffer[32];
	uint8_t dest[5] = {1, 10,0,0,2 };
	buffer[0] = 1;
	rf.queuePacket(dest, buffer);
	delay(100);
	buffer[0] = 0;
	rf.queuePacket(dest, buffer);
	delay(1000);	
}

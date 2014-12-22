#include <nrf24l01.h>

DSPI0 spi;

nrf24l01 rf(spi, 10, 9, 1);

void setup() {
	rf.begin(1, 10,0,0,2, 0);
	pinMode(PIN_LED1, OUTPUT);
}

void loop() {
	uint8_t buf[32];
	if (rf.available()) {
		rf.readPacket(buf);
		digitalWrite(PIN_LED1, buf[0]);
	}
}

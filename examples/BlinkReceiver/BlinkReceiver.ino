#include <nrf24l01.h>

DSPI0 spi;

// SPI port DSPI0, NCS is pin 10, CE is pin 9 and the interrupt is INT1
nrf24l01 rf(spi, 10, 9, 1);

void setup() {
    // Join mesh number 1, with my address 10,0,0,2 on channel 0.
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

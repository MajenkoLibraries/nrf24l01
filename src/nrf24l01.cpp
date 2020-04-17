#include <nrf24l01.h>

static nrf24l01 *isrObject0;
void nrf24l01_isrHandler0() { isrObject0->isrHandler(); }

static nrf24l01 *isrObject1;
void nrf24l01_isrHandler1() { isrObject1->isrHandler(); }

static nrf24l01 *isrObject2;
void nrf24l01_isrHandler2() { isrObject2->isrHandler(); }

static nrf24l01 *isrObject3;
void nrf24l01_isrHandler3() { isrObject3->isrHandler(); }

static nrf24l01 *isrObject4;
void nrf24l01_isrHandler4() { isrObject4->isrHandler(); }

static nrf24l01 *isrObject5;
void nrf24l01_isrHandler5() { isrObject5->isrHandler(); }

static nrf24l01 *isrObject6;
void nrf24l01_isrHandler6() { isrObject6->isrHandler(); }

static nrf24l01 *isrObject7;
void nrf24l01_isrHandler7() { isrObject7->isrHandler(); }

int nrf24l01::isrHandlerCounter = 0;

nrf24l01::nrf24l01(DSPI &spi, int csn, int ce, int intr) {
    _spi = &spi;
    _csn = csn;
    _ce = ce;
    _intr = intr;
    _status = 0;
}

void nrf24l01::begin(uint8_t ad0, uint8_t ad1, uint8_t ad2, uint8_t ad3, uint8_t ad4, uint8_t chan, uint8_t width) {
    _spi->begin();
    _addr[0] = ad0;
    _addr[1] = ad1;
    _addr[2] = ad2;
    _addr[3] = ad3;
    _addr[4] = ad4;
    _pipeWidth = width;
    pinMode(_csn, OUTPUT);
    pinMode(_ce, OUTPUT);
    pinMode(_intr, INPUT);
    digitalWrite(_csn, HIGH);
    digitalWrite(_ce, HIGH);
    setChannel(chan);
    regSet(REG_CONFIG, 2);
    uint8_t zero = 0x00;
    regWrite(REG_EN_AA, &zero, 1);
    _bc[0] = ad0;
    _bc[1] = 0;
    _bc[2] = 0;
    _bc[3] = 0;
    _bc[4] = 0;
    enablePipe(0, _bc);
    enablePipe(1, _addr);
    selectRX();
    switch (isrHandlerCounter) {
        case 0:
            isrObject0 = this;
            attachInterrupt(_intr, nrf24l01_isrHandler0, FALLING);
            break;
        case 1:
            isrObject1 = this;
            attachInterrupt(_intr, nrf24l01_isrHandler1, FALLING);
            break;
        case 2:
            isrObject2 = this;
            attachInterrupt(_intr, nrf24l01_isrHandler2, FALLING);
            break;
        case 3:
            isrObject3 = this;
            attachInterrupt(_intr, nrf24l01_isrHandler3, FALLING);
            break;
        case 4:
            isrObject4 = this;
            attachInterrupt(_intr, nrf24l01_isrHandler4, FALLING);
            break;
        case 5:
            isrObject5 = this;
            attachInterrupt(_intr, nrf24l01_isrHandler5, FALLING);
            break;
        case 6:
            isrObject6 = this;
            attachInterrupt(_intr, nrf24l01_isrHandler6, FALLING);
            break;
        case 7:
            isrObject7 = this;
            attachInterrupt(_intr, nrf24l01_isrHandler7, FALLING);
            break;
    }
    isrHandlerCounter++;
    digitalWrite(_csn, LOW);
    _status = _spi->transfer(CMD_TX_FLUSH);
    digitalWrite(_csn, HIGH);
    digitalWrite(_csn, LOW);
    _status = _spi->transfer(CMD_RX_FLUSH);
    digitalWrite(_csn, HIGH);

    
    uint8_t isrstat = 0x70;
    regWrite(REG_STATUS, &isrstat, 1);
    isrstat = 0xFF; 
    regWrite(REG_SETUP_RETR, &isrstat, 1);

    enablePower();
}

void nrf24l01::regRead(uint8_t reg, uint8_t *buffer, uint8_t len) {
    digitalWrite(_csn, LOW);
    _status = _spi->transfer(reg & 0x1F);
    for (int i = 0; i < len; i++) {
        buffer[i] = _spi->transfer(0xFF);
    }
    digitalWrite(_csn, HIGH);
}

void nrf24l01::regWrite(uint8_t reg, uint8_t *buffer, uint8_t len) {
    digitalWrite(_csn, LOW);
    _status = _spi->transfer((reg & 0x1F) | 0x20);
    for (int i = 0; i < len; i++) {
        _spi->transfer(buffer[i]);
    }
    digitalWrite(_csn, HIGH);
}

void nrf24l01::regSet(uint8_t reg, uint8_t bit) {
    uint8_t val;
    regRead(reg, &val, 1);
    val |= (1<<bit);
    regWrite(reg, &val, 1);
}

void nrf24l01::regClr(uint8_t reg, uint8_t bit) {
    uint8_t val;
    regRead(reg, &val, 1);
    val &= ~(1<<bit);
    regWrite(reg, &val, 1);
}

void nrf24l01::selectRX() {
    _mode = 0;
    regSet(REG_CONFIG, 0);
    digitalWrite(_ce, HIGH);
}

void nrf24l01::selectTX() {
    if (_mode == 1) {
        return;
    }
    _mode = 1;
    regClr(REG_CONFIG, 0);
    digitalWrite(_ce, LOW);
}

void nrf24l01::enablePipe(int pipe, uint8_t *addr) {
    uint8_t pw = 0x03;
    regSet(REG_EN_RXADDR, pipe);
    regWrite(REG_SETUP_AW, &pw, 1);
    regWrite(REG_RX_ADDR_P0 + pipe, addr, 5);
    regWrite(REG_RX_PW_P0 + pipe, &_pipeWidth, 1);
    regSet(REG_EN_AA, pipe);
}

void nrf24l01::enablePower() {
    regSet(REG_CONFIG, 1);
}

void nrf24l01::disablePower() {
    regClr(REG_CONFIG, 1);
}

void nrf24l01::isrHandler() {
    uint8_t isrstat = 0;
    uint8_t fifostat = 0;
    regRead(REG_STATUS, &isrstat, 1);
    regRead(REG_FIFO_STATUS, &fifostat, 1);


    // TX done
    if (isrstat & (1<<5)) {
        selectRX();
        enablePipe(0, _bc);
    }

    // Did we receive data?
    if (isrstat & (1<<6)) {
        // We're not actually going to do anything here.
    }

    if (isrstat & (1<<4)) {
        // Too many retries
        selectRX();
        enablePipe(0, _bc);
        digitalWrite(_csn, LOW);
        _status = _spi->transfer(CMD_TX_FLUSH);
        digitalWrite(_csn, HIGH);
    }

    // Clear interrupts
    isrstat = 0x70;
    regWrite(REG_STATUS, &isrstat, 1);
}

void nrf24l01::queuePacket(uint8_t *addr, uint8_t *packet) {
    uint8_t stat = 0;
    regRead(REG_FIFO_STATUS, &stat, 1);


    while (_mode == 1); // Wait for it to not be transmitting

    regWrite(REG_TX_ADDR, addr, 5);
    enablePipe(0, addr);
    selectTX();
    digitalWrite(_csn, LOW);
    _status = _spi->transfer(CMD_TX);
    for (int i = 0; i < _pipeWidth; i++) {
        _spi->transfer(packet[i]);
    }
    digitalWrite(_csn, HIGH);

    digitalWrite(_ce, HIGH);
    delayMicroseconds(20);
    digitalWrite(_ce, LOW);
}

int nrf24l01::available() {
    uint8_t fifostat = 0;
    regRead(REG_FIFO_STATUS, &fifostat, 1);
    if (fifostat & (1<<0)) {
        return 0;
    }
    return 1;
}

void nrf24l01::readPacket(uint8_t *buffer) {
    digitalWrite(_ce, LOW);
    digitalWrite(_csn, LOW);
    _status = _spi->transfer(CMD_RX);

    for (int i = 0; i < _pipeWidth; i++) {
        buffer[i] = _spi->transfer(0xFF);
    }
    digitalWrite(_csn, HIGH);
    digitalWrite(_ce, HIGH);
}

uint8_t nrf24l01::getStatus() {
    return _status;
}

void nrf24l01::setChannel(uint8_t chan) {
    regWrite(REG_RF_CH, &chan, 1);
}

void nrf24l01::setDataRate(uint8_t mhz) {
    switch (mhz) {
        case RATE_1MHZ:
            regClr(REG_RF_SETUP, 3);
            break;
        case RATE_2MHZ:
            regSet(REG_RF_SETUP, 3);
            break;
    }
}

void nrf24l01::setTXPower(uint8_t power) {
    switch (power) {
        case RF_TX_18DBM:
            regClr(REG_RF_SETUP, 1);
            regClr(REG_RF_SETUP, 2);
            break;
        case RF_TX_12DBM:
            regSet(REG_RF_SETUP, 1);
            regClr(REG_RF_SETUP, 2);
            break;
        case RF_TX_6DBM:
            regSet(REG_RF_SETUP, 1);
            regClr(REG_RF_SETUP, 2);
            break;
        case RF_TX_0DBM:
            regSet(REG_RF_SETUP, 1);
            regSet(REG_RF_SETUP, 2);
            break;
    }
}

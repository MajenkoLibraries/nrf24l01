#ifndef _NRF24L01_H
#define _NRF24L01_H

#if (ARDUINO >= 100)
# include <Arduino.h>
#else
# include <WProgram.h>
#endif

#include <DSPI.h>

#define DEFAULT_PIPE_WIDTH      32

#define CMD_REG_R       0x00
#define CMD_REG_W       0x20
#define CMD_RX          0x61
#define CMD_TX          0xA0
#define CMD_TX_FLUSH    0xE1
#define CMD_RX_FLUSH    0xE2
#define CMD_TX_REUSE    0xE3
#define CMD_NOP         0xFF

#define REG_CONFIG      0x00
#define REG_EN_AA       0x01
#define REG_EN_RXADDR   0x02
#define REG_SETUP_AW    0x03
#define REG_SETUP_RETR  0x04
#define REG_RF_CH       0x05
#define REG_RF_SETUP    0x06
#define REG_STATUS      0x07
#define REG_OBSERVE_TX  0x08
#define REG_CD          0x09
#define REG_RX_ADDR_P0  0x0A
#define REG_RX_ADDR_P1  0x0B
#define REG_RX_ADDR_P2  0x0C
#define REG_RX_ADDR_P3  0x0D
#define REG_RX_ADDR_P4  0x0E
#define REG_RX_ADDR_P5  0x0F
#define REG_TX_ADDR     0x10
#define REG_RX_PW_P0    0x11
#define REG_RX_PW_P1    0x12
#define REG_RX_PW_P2    0x13
#define REG_RX_PW_P3    0x14
#define REG_RX_PW_P4    0x15
#define REG_RX_PW_P5    0x16
#define REG_FIFO_STATUS 0x17

#define RATE_1MHZ       1
#define RATE_2MHZ       2

#define RF_TX_18DBM     0
#define RF_TX_12DBM     1
#define RF_TX_6DBM      2
#define RF_TX_0DBM      3

class nrf24l01 {

    private:
        static int isrHandlerCounter;

        DSPI *_spi;
        int _csn;
        int _ce;
        int _intr;
        int _status;
        uint8_t _addr[5];
        uint8_t _bc[5];
        uint8_t _mode;
        uint8_t _pipeWidth;

        void regRead(uint8_t reg, uint8_t *buffer, uint8_t len);
        void regWrite(uint8_t reg, uint8_t *buffer, uint8_t len);
        void regSet(uint8_t reg, uint8_t bit);
        void regClr(uint8_t reg, uint8_t bit);
        void selectRX();
        void selectTX();

    public:
        nrf24l01(DSPI &spi, int csn, int ce, int intr);
        void begin(uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4, uint8_t chan, uint8_t width = DEFAULT_PIPE_WIDTH);
        void enablePipe(int pipe, uint8_t *addr);
        void enablePower();
        void disablePower();
        void isrHandler();
        void queuePacket(uint8_t *addr, uint8_t *data);
        int available();
        void readPacket(uint8_t *buffer);
        uint8_t getStatus();
        void setChannel(uint8_t chan);
        void setDataRate(uint8_t mhz);
        void setTXPower(uint8_t power);
};

#endif

#ifndef MCP23S17_H
#define MCP23S17_H

#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <pigpio.h>

// Datasheet: https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf

class MCP23S17
{
public:
    enum class PORT
    {
        A = 0,
        B = 1
    };

    enum class DIRECTION
    {
        OUTPUT = 0,
        INPUT = 1
    };

private:
    enum class OPERATION
    {
        READ = 1,
        WRITE = 0
    };

    enum class ADDR_BANK_0
    {
        IODIRA = 0x00,
        IODIRB = 0x01,
        IPOLA = 0x02,
        IPOLB = 0x03,
        GPINTENA = 0x04,
        GPINTENB = 0x05,
        DEFVALA = 0x06,
        DEFVALB = 0x07,
        INTCONA = 0x08,
        INTCONB = 0x09,
        IOCON = 0x0A,
        //  IOCON     = 0x0B // IOCON is shared between ports A and B
        GPPUA = 0x0C,
        GPPUB = 0x0D,
        INTFA = 0x0E,
        INTFB = 0x0F,
        INTCAPA = 0x10,
        INTCAPB = 0x11,
        GPIOA = 0x12,
        GPIOB = 0x13,
        OLATA = 0x14,
        OLATB = 0x15
    };

private:
    static constexpr uint8_t MCP23S17_ADDR = 0x40;
    static constexpr uint32_t SPI_SPEED = 250000;

    uint8_t spiHandle;
    uint8_t SPI_CE, SPI_MISO, SPI_MOSI, SPI_SCLK;

private:
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);

public:
    MCP23S17(uint8_t cs, uint8_t miso, uint8_t mosi, uint8_t sclk, uint32_t baud = SPI_SPEED);
    ~MCP23S17();

    void direction(PORT port, DIRECTION direction);
    void write(PORT port, uint8_t data);
    uint8_t read(PORT port);

    void enablePullup(PORT port, uint8_t mask);
    void delay_ms(int milliseconds);
};

#endif

// EOF

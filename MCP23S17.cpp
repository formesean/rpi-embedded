#include "MCP23S17.hpp"

#include <bitset>
#include <iostream>
#include <stdexcept>
#include <pigpio.h>

MCP23S17::MCP23S17(uint8_t hardware_address, uint8_t sclk, uint8_t mosi, uint8_t miso, uint8_t cs, uint32_t baud)
    : m_hardware_address(hardware_address), m_spi_cs(cs)
{
    if (gpioInitialise() < 0)
    {
        throw std::runtime_error("pigpio initialization failed!");
    }

    // Open the SPI bit-bang channel using pigpio.
    int status = bb_spi_open(sclk, mosi, miso, cs, baud);
    if (status < 0)
    {
        std::cerr << "Failed to open bit-bang SPI on CS " << cs
                  << " using SCLK: " << sclk
                  << ", MOSI: " << mosi
                  << ", MISO: " << miso
                  << " with baud " << baud << std::endl;
        throw std::runtime_error("bb_spi_open failed with error: " + std::to_string(status));
    }
}

MCP23S17::~MCP23S17()
{
    // Close the SPI bit-bang channel.
    bb_spi_close(m_spi_cs);
    gpioTerminate();
}

uint8_t MCP23S17::control_byte(MCP23S17::OPERATION op) const
{
    return (
        (CLIENT_ADDRESS_HEADER & 0xF0) |
        ((m_hardware_address & 0x07) << 1) |
        static_cast<bool>(op));
}

uint8_t MCP23S17::register_address(MCP23S17::REGISTER reg, MCP23S17::PORT port) const
{
    if (m_IOCON_BANK)
    {
        return ((static_cast<bool>(port) << 4) | (static_cast<uint8_t>(reg) & 0x0F));
    }
    else
    {
        return ((static_cast<uint8_t>(reg) * 2) + (static_cast<bool>(port)));
    }
}

uint8_t MCP23S17::mcp_xfer(MCP23S17::OPERATION op,
                           MCP23S17::REGISTER reg,
                           MCP23S17::PORT port,
                           uint8_t data)
{
    uint8_t rxd[3] = {0};
    uint8_t txd[3] =
        {
            control_byte(op),
            register_address(reg, port),
            data};

    spi_xfer(rxd, txd, 3);
    return rxd[2];
}

void MCP23S17::direction(MCP23S17::PORT port, MCP23S17::DIRECTION direction)
{
    mcp_xfer(
        MCP23S17::OPERATION::WRITE,
        MCP23S17::REGISTER::IODIR,
        port,
        static_cast<bool>(direction) ? 0xFF : 0x00);
}

uint8_t MCP23S17::read(MCP23S17::PORT port)
{
    return mcp_xfer(MCP23S17::OPERATION::READ, MCP23S17::REGISTER::GPIO, port);
}

void MCP23S17::write(MCP23S17::PORT port, uint8_t data)
{
    mcp_xfer(MCP23S17::OPERATION::WRITE, MCP23S17::REGISTER::GPIO, port, data);
}

void MCP23S17::spi_xfer(uint8_t *rxd, uint8_t *txd, unsigned length)
{
    int ret = bb_spi_xfer(m_spi_cs, reinterpret_cast<char *>(txd), reinterpret_cast<char *>(rxd), length);
    if (ret < 0)
    {
        std::cerr << "bb_spi_xfer failed with error: " << ret << std::endl;
        throw std::runtime_error("bb_spi_xfer failed with error: " + std::to_string(ret));
    }
}

// EOF

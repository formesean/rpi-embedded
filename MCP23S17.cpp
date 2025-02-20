#include "MCP23S17.hpp"

MCP23S17::MCP23S17(uint8_t cs, uint8_t miso, uint8_t mosi, uint8_t sclk, uint32_t baud)
    : SPI_CE(cs), SPI_MISO(miso), SPI_MOSI(mosi), SPI_SCLK(sclk)
{
    if (gpioInitialise() < 0)
    {
        throw std::runtime_error("pigpio initialization failed!");
    }

    int status = bbSPIOpen(cs, miso, mosi, sclk, baud, 0);
    if (status < 0)
    {
        gpioTerminate();
        throw std::runtime_error("Failed to open SPI!");
    }
}

MCP23S17::~MCP23S17()
{
    int closeStatus = bbSPIClose(SPI_CE);
    if (closeStatus != 0)
    {
        std::cerr << "Failed to close SPI interface. Error code: " << closeStatus << "\n";
    }
    gpioTerminate();
}

void MCP23S17::direction(PORT port, DIRECTION direction)
{
    uint8_t reg = (port == PORT::A) ? static_cast<uint8_t>(ADDR_BANK_0::IODIRA) : static_cast<uint8_t>(ADDR_BANK_0::IODIRB);
    writeRegister(reg, direction == DIRECTION::INPUT ? 0xFF : 0x00);
}

void MCP23S17::write(PORT port, uint8_t data)
{
    uint8_t reg = (port == PORT::A) ? static_cast<uint8_t>(ADDR_BANK_0::OLATA) : static_cast<uint8_t>(ADDR_BANK_0::OLATB);
    writeRegister(reg, data);
}

uint8_t MCP23S17::read(PORT port)
{
    uint8_t reg = (port == PORT::A) ? static_cast<uint8_t>(ADDR_BANK_0::GPIOA) : static_cast<uint8_t>(ADDR_BANK_0::GPIOB);
    return readRegister(reg);
}

void MCP23S17::writeRegister(uint8_t reg, uint8_t value)
{
    uint8_t buffer[3] = {MCP23S17_ADDR, reg, value};
    uint8_t rx_buffer[3] = {0};

    int status = bbSPIXfer(SPI_CE, (char *)buffer, (char *)rx_buffer, 3);
    if (status < 0 || status != 3)
    {
        std::cerr << "SPI Write Failed (status: " << status << ")\n";
    }
}

uint8_t MCP23S17::readRegister(uint8_t reg)
{
    uint8_t txBuf[3] = {(uint8_t)(MCP23S17_ADDR | 0x01), reg, 0x00};
    uint8_t rxBuf[3] = {0};

    int status = bbSPIXfer(SPI_CE, (char *)txBuf, (char *)rxBuf, 3);
    if (status < 0 || status != 3)
    {
        std::cerr << "SPI Read Failed (status: " << status << ")\n";
        return 0xFF;
    }
    return rxBuf[2]; // Received data is in the 3rd byte
}

void MCP23S17::enablePullup(PORT port, uint8_t mask)
{
    uint8_t reg = (port == PORT::A) ? static_cast<uint8_t>(ADDR_BANK_0::GPPUA) : static_cast<uint8_t>(ADDR_BANK_0::GPPUB);
    writeRegister(reg, mask);
}

void MCP23S17::delay_ms(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// EOF

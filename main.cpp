#include <iostream>
#include <cstdint>
#include <pigpio.h>

class MCP23S17
{
private:
    // SPI channel and speed configuration
    static constexpr uint8_t SPI_CE = 16;
    static constexpr uint8_t SPI_MISO = 19;
    static constexpr uint8_t SPI_MOSI = 20;
    static constexpr uint8_t SPI_SCLK = 21;
    static constexpr uint32_t SPI_SPEED = 250000; // 2.5 KHz
    static constexpr uint8_t MCP23S17_ADDR = 0x40;

    // MCP23S17 Registers
    static constexpr uint8_t IODIRA = 0x00;
    static constexpr uint8_t IODIRB = 0x01;
    static constexpr uint8_t IOCON = 0x0A;
    static constexpr uint8_t GPIOA = 0x12;
    static constexpr uint8_t GPIOB = 0x13;
    static constexpr uint8_t OLATA = 0x14;
    static constexpr uint8_t OLATB = 0x15;

    int spiHandle;

public:
    MCP23S17()
    {
        if (gpioInitialise() < 0)
        {
            throw std::runtime_error("pigpio initialization failed!");
        }

        spiHandle = bbSPIOpen(SPI_CE, SPI_MISO, SPI_MOSI, SPI_SCLK, SPI_SPEED, 0);
        if (spiHandle < 0)
        {
            gpioTerminate();
            throw std::runtime_error("Failed to open SPI!");
        }

        setup();
    }

    ~MCP23S17()
    {
        bbSPIClose(SPI_CE);
        gpioTerminate();
    }

    void writeRegister(uint8_t reg, uint8_t value)
    {
        uint8_t buffer[3] = {MCP23S17_ADDR, reg, value};
        uint8_t rx_buffer[3] = {0};

        int status = bbSPIXfer(SPI_CE, (char *)buffer, (char *)rx_buffer, 3);

        if (status < 0)
            std::cerr << "SPI Write Failed (status: " << status << ")\n";
        else
            std::cerr << "SPI Write Successful (status: " << status << ")\n";
    }

    uint8_t readRegister(uint8_t reg)
    {
        uint8_t txBuf[3] = {(uint8_t)(MCP23S17_ADDR | 0x01), reg, 0x00};
        uint8_t rxBuf[3] = {0};

        int status = bbSPIXfer(SPI_CE, (char *)txBuf, (char *)rxBuf, 3);

        if (status < 0)
        {
            std::cerr << "SPI Read Failed (status: " << status << ")\n";
            return 0xFF;
        }
        else
        {
            std::cerr << "SPI Read Successful (status: " << status << ")\n";
            return rxBuf[2]; // Received data is in the 3rd byte
        }
    }

    void writeGPIOA(uint8_t value)
    {
        writeRegister(OLATA, value);
    }

    uint8_t readGPIOB(void)
    {
        return readRegister(GPIOB);
    }

    void setup()
    {
        writeRegister(IODIRA, 0x00); // Set PORTA as outputs
        writeRegister(IODIRB, 0xFF); // Set PORTB as inputs
        writeRegister(IOCON, 0x08);  // Enable hardware address pins
        writeGPIOA(0x00);
    }
};

int main()
{
    try
    {
        MCP23S17 mcp;

        while (true)
        {
            mcp.writeGPIOA(0x01);
            gpioDelay(500000);
            mcp.writeGPIOA(0x00);
            gpioDelay(500000);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
    return 0;
};

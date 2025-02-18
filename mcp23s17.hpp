#ifndef MCP23S17_HPP
#define MCP23S17_HPP

#include <pigpio.h>
#include <cstdint>

class MCP23S17
{
public:
    explicit MCP23S17(int spiChannel, int spiSpeed = 1000000, int csPin = 16);
    ~MCP23S17();

    void writeRegister(uint8_t reg, uint8_t value);
    void readRegister(uint8_t reg);
    void setPinMode(uint8_t pin, bool isOutput);
    void digitalWrite(uint8_t pin, bool value);

private:
    int spiHandle;
    int csPin;
    static constexpr uint8_t MCP23S17_WRITE = 0x40;
    static constexpr uint8_t MCP23S17_READ = 0x41;

    void spiTransfer(uint8_t reg, uint8_t value, bool isRead);
}

#endif

// EOF

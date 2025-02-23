#include "../lib/AikaPi/AikaPi.h"
#include "../lib/MCP23S17/MCP23S17.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include <pigpio.h>

#define CS_PIN 16
#define MISO_PIN 19
#define MOSI_PIN 20
#define SCLK_PIN 21

class MCP23S17_pigpio : public MCP23S17 {
private:
  AikaPi::SPI_BB m_spi_bb;

public:
  MCP23S17_pigpio(int CS, int MISO, int MOSI, int SCLK, double baud,
                  uint8_t hw_addr_bits)
      : MCP23S17(hw_addr_bits),
        m_spi_bb(AikaPi::SPI_BB(CS, MISO, MOSI, SCLK, baud)) {}

  void spi_xfer(uint8_t *rxd, uint8_t *txd, unsigned length) {
    m_spi_bb.xfer((char *)rxd, (char *)txd, length);
  }
};

int main() {
  gpioInitialise();

  MCP23S17_pigpio mcp(CS_PIN, MISO_PIN, MOSI_PIN, SCLK_PIN, 150000, 0x00);

  mcp.direction(MCP23S17_pigpio::PORT::A, MCP23S17_pigpio::DIRECTION::OUTPUT);
  mcp.direction(MCP23S17_pigpio::PORT::B, MCP23S17_pigpio::DIRECTION::INPUT);

  while (true) {
    uint8_t switchState = mcp.read(MCP23S17::PORT::B);

    if (switchState == 1)
      mcp.write(MCP23S17_pigpio::PORT::A, 0x01);
    else
      mcp.write(MCP23S17_pigpio::PORT::A, 0x00);
  }

  gpioTerminate();
}

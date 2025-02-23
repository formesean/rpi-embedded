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

#define ROW1 0b00000001
#define ROW2 0b00000010
#define COL1 0b00000001
#define COL2 0b00000010

class MCP23S17_pigpio : public MCP23S17
{
private:
  AikaPi::SPI_BB m_spi_bb;

public:
  MCP23S17_pigpio(int CS, int MISO, int MOSI, int SCLK, double baud,
                  uint8_t hw_addr_bits)
      : MCP23S17(hw_addr_bits),
        m_spi_bb(AikaPi::SPI_BB(CS, MISO, MOSI, SCLK, baud)) {}

  void spi_xfer(uint8_t *rxd, uint8_t *txd, unsigned length)
  {
    m_spi_bb.xfer((char *)rxd, (char *)txd, length);
  }
};

int main()
{
  gpioInitialise();

  MCP23S17_pigpio mcp(CS_PIN, MISO_PIN, MOSI_PIN, SCLK_PIN, 150000, 0x00);

  mcp.direction(MCP23S17_pigpio::PORT::A, MCP23S17_pigpio::DIRECTION::INPUT);
  mcp.direction(MCP23S17_pigpio::PORT::B, MCP23S17_pigpio::DIRECTION::OUTPUT);
  mcp.enablePullup(MCP23S17_pigpio::PORT::A, COL1 | COL2);

  while (true)
  {
    for (uint8_t row = 0; row < 2; row++)
    {
      // Drive only one row low at a time
      uint8_t rowMask = (row == 0) ? ROW1 : ROW2;
      mcp.write(MCP23S17_pigpio::PORT::B, ~rowMask);

      uint8_t colState = mcp.read(MCP23S17_pigpio::PORT::A);

      // Check which keys are pressed
      if (!(colState & COL1))
      {
        std::cout << "Key Pressed: Row " << row + 1 << " Col 1" << std::endl;
      }
      if (!(colState & COL2))
      {
        std::cout << "Key Pressed: Row " << row + 1 << " Col 2" << std::endl;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  gpioTerminate();
}

// EOF

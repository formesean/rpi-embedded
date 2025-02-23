#ifndef SPICO_HPP
#define SPICO_HPP

#include <pico/stdlib.h>
#include <hardware/spi.h>
#include <cstdint>
#include <stdio.h>

class SPICO
{
public:
  enum class DIRECTION
  {
    INPUT = 0,
    OUTPUT = 1
  };

  enum class PORT
  {
    A = 0,
    B = 1
  };

private:
  enum class MCP23S17_REGISTER
  {
    IODIR = 0,
    IPOL = 1,
    GPINTEN = 2,
    DEFVAL = 3,
    INTCON = 4,
    IOCON = 5,
    GPPU = 6,
    INTF = 7,
    INTCAP = 8,
    GPIO = 9,
    OLAT = 0xA,
  };

  enum class MCP23S17_ADDR_BANK_0
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
    INTFB = 0x0C,
    INTCAPA = 0x10,
    INTCAPB = 0x11,
    GPIOA = 0x12,
    GPIOB = 0x13,
    OLATA = 0x14,
    OLATB = 0x15
  };

public:
  SPICO(spi_inst_t *spi_port = spi0, uint32_t baudrate = 500000, uint8_t cs_pin = 5);

  // SPI Functions
  void begin(uint8_t miso, uint8_t mosi, uint8_t sclk);
  uint8_t transfer(uint8_t data);
  void setClockSpeed(uint32_t baudrate);

  // GPIO Functions
  static void pinMode(uint8_t pin, DIRECTION dir);
  static void digitalWrite(uint8_t pin, bool value);
  static bool digitalRead(uint8_t pin);

  // MCP23S17 Functions
  void mcp23s17_pinMode(PORT port, DIRECTION dir);
  void mcp23s17_digitalWrite(PORT port, uint8_t data);
  uint8_t mcp23s17_digitalRead(PORT port);

  void delay(uint32_t ms);

private:
  spi_inst_t *spi;
  uint8_t spi_baudrate;
  uint8_t cs_pin;
  static constexpr uint8_t MCP23S17_ADDR = 0x40; // MCP23S17 base address

  void mcp23s17_write(uint8_t reg, uint8_t data);
  uint8_t mcp23s17_read(uint8_t reg);
};

#endif

// EOF

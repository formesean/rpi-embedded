#include "../../lib/SPICO/SPICO.hpp"

#define PIN_MISO 16 // RX
#define PIN_CS 17
#define PIN_SCLK 18
#define PIN_MOSI 19 // TX

#define MCP23S17_ADDR 0x40
#define IODIRA 0x00
#define GPIOA 0x12
#define OLATA 0x14

int main()
{
  stdio_init_all();

  SPICO spi(spi0, 500000, PIN_CS);
  spi.begin(PIN_MISO, PIN_MOSI, PIN_SCLK);

  spi.mcp23s17_pinMode(SPICO::PORT::A, SPICO::DIRECTION::OUTPUT);
  spi.mcp23s17_pinMode(SPICO::PORT::B, SPICO::DIRECTION::INPUT);

  while (true)
  {
    uint8_t switchState = spi.mcp23s17_digitalRead(SPICO::PORT::B);

    if (switchState == 0x01)
      spi.mcp23s17_digitalWrite(SPICO::PORT::A, 0x01);
    else
      spi.mcp23s17_digitalWrite(SPICO::PORT::A, 0x00);
  }
}

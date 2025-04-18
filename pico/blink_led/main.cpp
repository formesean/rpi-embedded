#include "../../lib/SPICO/SPICO.hpp"

#define LED_PIN 25 // Pico onboard LED

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

  SPICO spi(spi0, 1000000, PIN_CS);
  spi.begin(PIN_MISO, PIN_MOSI, PIN_SCLK);

  spi.pinMode(LED_PIN, SPICO::DIRECTION::OUTPUT);

  spi.mcp23s17_pinMode(SPICO::PORT::A, SPICO::DIRECTION::OUTPUT);

  while (true)
  {
    spi.mcp23s17_digitalWrite(SPICO::PORT::A, 0x01);
    spi.digitalWrite(LED_PIN, 1);
    spi.delay(500);

    spi.mcp23s17_digitalWrite(SPICO::PORT::A, 0x00);
    spi.digitalWrite(LED_PIN, 0);
    spi.delay(500);
  }
}

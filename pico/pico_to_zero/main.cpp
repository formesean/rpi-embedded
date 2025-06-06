#include "pico/stdlib.h"
#include "hardware/spi.h"

#include <cstdint>

#define PACKET_SIZE 4

constexpr uint32_t SPI_BAUD = 1000000;
constexpr uint8_t PIN_SCK = 10;
constexpr uint8_t PIN_MISO = 11;
constexpr uint8_t PIN_MOSI = 12;
constexpr uint8_t PIN_CS = 13;

void spi_slave_init();
void sendTestData();
void sendBufferData(const uint8_t *buffer);

int main()
{
  stdio_init_all();
  spi_slave_init();

  while (true)
  {
    while (gpio_get(PIN_CS))
      tight_loop_contents();

    sendTestData();

    while (!gpio_get(PIN_CS))
      tight_loop_contents();
  }
}

void spi_slave_init()
{
  spi_init(spi1, SPI_BAUD);
  spi_set_slave(spi1, true);

  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
}

void sendTestData()
{
  uint8_t data_out[4] = {0xA1, 0xB2, 0xC3, 0xD4};
  uint8_t data_in[4] = {0};
  spi_write_read_blocking(spi1, data_out, data_in, 4);
}

void sendBufferData(const uint8_t *buffer)
{
  uint8_t data_in[4] = {0};
  spi_write_read_blocking(spi1, (uint8_t *)buffer, data_in, 4);
}

// EOF

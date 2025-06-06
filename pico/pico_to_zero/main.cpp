#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <hardware/irq.h>
#include <hardware/sync.h>
#include <stdio.h>
#include <cstdint>
#include <cstdlib>

#define SPI_PORT spi1
#define SPI_BAUD 1000000
#define SCLK_PIN 10
#define MOSI_PIN 11 // TX
#define MISO_PIN 12 // RX
#define CS_PIN 13

#define PACKET_SIZE 4

const uint8_t packet[PACKET_SIZE] = {
    0x01,              // Type
    0x02,              // Action
    0x01,              // Value
    0x01 ^ 0x02 ^ 0x01 // Checksum
};

void spi_slave_task()
{
  uint8_t index = 0;

  while (true)
  {
    if (spi_is_readable(SPI_PORT))
    {
      // Always preload TX FIFO first
      if (spi_is_writable(SPI_PORT))
        spi_get_hw(SPI_PORT)->dr = packet[index];

      volatile uint8_t discard = spi_get_hw(SPI_PORT)->dr;

      index = (index + 1) % PACKET_SIZE;
    }
  }
}

int main()
{
  stdio_init_all();

  spi_init(SPI_PORT, SPI_BAUD);
  spi_set_slave(spi1, true);
  gpio_set_function(MISO_PIN, GPIO_FUNC_SPI);
  gpio_set_function(SCLK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
  gpio_set_function(CS_PIN, GPIO_FUNC_SPI);

  spi_slave_task();
  return 0;
}

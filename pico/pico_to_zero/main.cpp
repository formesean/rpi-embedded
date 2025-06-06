#include "../../lib/SPICO/SPICO.hpp"

#include <string.h>

#define SPI_PORT spi1
#define SPI_BAUD 1000000
#define SCLK_PIN 10
#define MOSI_PIN 11 // TX
#define MISO_PIN 12 // RX
#define CS_PIN 13

#define PACKET_SIZE 4

void spi_slave_task()
{
  uint8_t tx_buffer[PACKET_SIZE] = {0x01, 0x02, 0x01, 0x02}; // Type, Action, Value, Checksum
  uint8_t rx_buffer[PACKET_SIZE] = {0};

  // Calculate checksum
  tx_buffer[3] = tx_buffer[0] ^ tx_buffer[1] ^ tx_buffer[2];

  memcpy(tx_buffer, message, PACKET_SIZE); // Load message

  while (true)
  {
    spi_write_blocking(SPI_PORT, tx_buffer, PACKET_SIZE);
    spi_read_blocking(SPI_PORT, 0, rx_buffer, PACKET_SIZE);
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

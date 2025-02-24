#include "../../lib/SPICO/SPICO.hpp"

#include <string.h>

#define SPI_PORT spi1
#define SPI_BAUD 250000
#define SCLK_PIN 10
#define MOSI_PIN 11 // TX
#define MISO_PIN 12 // RX
#define CS_PIN 13

#define BUFFER_SIZE 17
const char message[BUFFER_SIZE] = "Hello from Pico!";

void spi_slave_task()
{
  uint8_t tx_buffer[BUFFER_SIZE];       // Transmit buffer
  uint8_t rx_buffer[BUFFER_SIZE] = {0}; // Receive buffer

  memcpy(tx_buffer, message, BUFFER_SIZE); // Load message

  while (true)
  {
    spi_write_blocking(SPI_PORT, tx_buffer, BUFFER_SIZE);
    spi_read_blocking(SPI_PORT, 0, rx_buffer, BUFFER_SIZE);
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

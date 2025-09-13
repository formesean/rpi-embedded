#include "pico/stdlib.h"
#include "hardware/spi.h"

#include <stdio.h>
#include <cstdint>

#define PACKET_SIZE 24

constexpr uint32_t SPI_BAUD = 20000000;
constexpr uint8_t PIN_SCK = 10;
constexpr uint8_t PIN_MOSI = 11;
constexpr uint8_t PIN_MISO = 12;
constexpr uint8_t PIN_CS = 13;

void wait_for_usb_connect();
void spi_slave_init();
void receive_frames_blocking();

int main()
{
  wait_for_usb_connect();
  spi_slave_init();

  printf("SPI1 slave ready on GP10/11/12, CSn GP13. Waiting for master...\n");
  fflush(stdout);

  receive_frames_blocking();
}

void wait_for_usb_connect()
{
  stdio_init_all();

  absolute_time_t timeout = make_timeout_time_ms(5000);
  while (!stdio_usb_connected() && !time_reached(timeout))
    sleep_ms(10);

  sleep_ms(100);
}

void spi_slave_init()
{
  spi_init(spi1, SPI_BAUD);
  // Match RPi master: 8-bit, mode 0 (CPOL=0, CPHA=0), MSB first
  spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
  spi_set_slave(spi1, true);

  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_CS, GPIO_FUNC_SPI);

  // Clear any stale RX data
  while (spi_is_readable(spi1)) {
    (void)spi_get_hw(spi1)->dr;
  }
}

void receive_frames_blocking()
{
  uint8_t rx_buffer[PACKET_SIZE];
  const uint8_t tx_fill = 0x00;

  while (true)
  {
    int read_count = spi_read_blocking(spi1, tx_fill, rx_buffer, PACKET_SIZE);
    if (read_count == PACKET_SIZE)
    {
      printf("RX (%d bytes): ", read_count);
      for (int i = 0; i < PACKET_SIZE; ++i)
      {
        printf("%02X", rx_buffer[i]);
        if (i + 1 < PACKET_SIZE) printf(" ");
      }
      printf("\n");
    }
    else
    {
      printf("RX error, bytes: %d\n", read_count);
    }
    fflush(stdout);
  }
}

// EOF

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include <stdio.h>
#include <iostream>
#include <cstdint>

#define PACKET_SIZE 4

constexpr uint32_t SPI_BAUD = 1000000;
constexpr uint8_t PIN_SCK = 10;
constexpr uint8_t PIN_MISO = 11;
constexpr uint8_t PIN_MOSI = 12;
constexpr uint8_t PIN_CS = 13;

void wait_for_usb_connect();
void spi_slave_init();
void sendTestData();
void sendBufferData(const uint8_t *buffer);

int main()
{
  wait_for_usb_connect();
  spi_slave_init();

  while (true)
  {
    sendTestData();
    tight_loop_contents();
  }
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
  spi_set_slave(spi1, true);

  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
}

void sendTestData()
{
  uint8_t ready_packet[4] = {0x01, 0x02, 0x01, 0x01 ^ 0x02 ^ 0x01};
  uint8_t high_byte = ((ready_packet[0] & 0x0F) << 4) | (ready_packet[1] & 0x0F);
  uint8_t low_byte = ((ready_packet[2] & 0x0F) << 4) | (ready_packet[3] & 0x0F);
  uint16_t packed = (high_byte << 8) | low_byte;

  spi_write16_blocking(spi1, &packed, PACKET_SIZE);

  printf("Packed value: 0x%04X\n", packed);
  fflush(stdout);
}

void sendBufferData(const uint8_t *buffer)
{
  uint8_t data_in[4] = {0};
  spi_write_read_blocking(spi1, (uint8_t *)buffer, data_in, 4);
}

// EOF

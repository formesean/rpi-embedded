#include "pico/stdlib.h"
#include "hardware/spi.h"

#include <stdio.h>
#include <cstdint>

#define PACKET_SIZE 2

constexpr uint32_t SPI_BAUD = 20000000;
constexpr uint8_t PIN_SCK  = 10;
// In slave mode on RP2040: TX=GP11 (MISO), RX=GP12 (MOSI)
constexpr uint8_t PIN_MOSI = 12;
constexpr uint8_t PIN_MISO = 11;
constexpr uint8_t PIN_CS = 13;

void wait_for_usb_connect();
void spi_slave_init();
void receiveTestData();

int main()
{
  wait_for_usb_connect();
  spi_slave_init();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, true);

  while (true)
  {
    receiveTestData();
    gpio_xor_mask(1u << PICO_DEFAULT_LED_PIN);
    sleep_ms(100);
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
  spi_set_format(spi1, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
  spi_set_slave(spi1, true);

  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
}

void receiveTestData()
{
  uint16_t tx_dummy = 0x0000;
  uint16_t rx_word  = 0x0000;

  int result = spi_write16_read16_blocking(spi1, &tx_dummy, &rx_word, 1);

  if (result == 1)
  {
    uint8_t rx_high = static_cast<uint8_t>((rx_word >> 8) & 0xFF);
    uint8_t rx_low  = static_cast<uint8_t>(rx_word & 0xFF);

    printf("[SLAVE] RX word: 0x%04X (bytes: 0x%02X 0x%02X)\n",
           rx_word, rx_high, rx_low);
  }
  else
  {
    printf("[SLAVE] SPI receive failed\n");
  }

  fflush(stdout);
}

// EOF

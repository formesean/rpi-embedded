#include "../lib/AikaPi/AikaPi.h"

#include <iostream>
#include <iomanip>

#define CS_PIN 16
#define MISO_PIN 19
#define MOSI_PIN 20
#define SCLK_PIN 21

#define BUFFER_SIZE 17

int main()
{
  // TODO
  // fix errors: no bytes received from PICO

  AikaPi &rpi = AikaPi::get_instance();

  // Enable AUX SPI1 peripheral
  rpi.aux.master_enable_spi(1);

  // GPIO pins for SPI1
  rpi.gpio.set(SCLK_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  rpi.gpio.set(MOSI_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  rpi.gpio.set(MISO_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::DOWN);
  rpi.gpio.set(CS_PIN, AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::OFF);

  auto &spi1 = rpi.aux.spi(1);

  // SPI settings
  spi1.mode(AP::SPI::MODE::_0);
  spi1.frequency(1000000);
  spi1.shift_in_ms_bit_first(true);
  spi1.cs_polarity(true);
  spi1.cs(2);
  spi1.clear_fifos();

  while (true)
  {
    char tx_buffer[BUFFER_SIZE] = {0}; // Send dummy bytes
    char rx_buffer[BUFFER_SIZE] = {0}; // Receive buffer

    spi1.xfer(rx_buffer, tx_buffer, BUFFER_SIZE);

    std::cout << "Received: ";
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
      std::cout << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(rx_buffer[i]) << " ";
    }
    std::cout << std::dec << "\n"
              << std::endl;
  }

  return 0;
}

// EOF

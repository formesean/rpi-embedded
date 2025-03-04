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
  AikaPi &rpi = AikaPi::get_instance();

  // Enable AUX SPI1 peripheral
  rpi.aux.master_enable_spi(1);

  // GPIO pins for SPI1
  rpi.gpio.set(SCLK_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  rpi.gpio.set(MOSI_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  rpi.gpio.set(MISO_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::DOWN);

  auto &spi1 = rpi.aux.spi(1);

  // SPI settings
  spi1.mode(AP::SPI::MODE::_0);
  spi1.frequency(1000000);
  spi1.shift_in_ms_bit_first(true);
  spi1.cs_polarity(true);
  spi1.clear_fifos();

  while (true)
  {
    char tx_buffer[BUFFER_SIZE];
    char rx_buffer[BUFFER_SIZE] = {0};

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
      tx_buffer[i] = static_cast<char>(i);
    }

    spi1.xfer(rx_buffer, tx_buffer, BUFFER_SIZE, 2);

    bool match = true;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
      if (rx_buffer[i] != tx_buffer[i])
      {
        match = false;
        break;
      }
    }

    // Display results
    std::cout << "TX: ";
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
      std::cout << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(tx_buffer[i]) << " ";
    }

    std::cout << "\nRX: ";
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
      std::cout << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(rx_buffer[i]) << " ";
    }

    std::cout << "\nMatch: " << (match ? "YES" : "NO")
              << "\n"
              << std::endl;
  }

  return 0;
}

// EOF

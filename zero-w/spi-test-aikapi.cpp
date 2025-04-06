#include "../lib/AikaPi/AikaPi.h"

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

#define CS_PIN 16
#define MISO_PIN 19
#define MOSI_PIN 20
#define SCLK_PIN 21
#define BAUD_RATE 500

#define BUFFER_SIZE 17

int main()
{
  AikaPi &rpi = AikaPi::get_instance();

  // Enable AUX SPI1 peripheral
  rpi.aux.master_enable_spi(0);

  // GPIO pins for SPI1
  rpi.gpio.set(SCLK_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  rpi.gpio.set(MOSI_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::DOWN);
  rpi.gpio.set(MISO_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);

  auto &spi1 = rpi.aux.spi(0);

  // SPI settings
  spi1.enable();
  spi1.frequency(BAUD_RATE);

  while (true)
  {
    char tx_buffer[BUFFER_SIZE];
    char rx_buffer[BUFFER_SIZE] = {0};

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
      tx_buffer[i] = static_cast<char>(i);
    }

    spi1.xfer(rx_buffer, tx_buffer, BUFFER_SIZE);

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

    int delay_ms = (BUFFER_SIZE * 8 * 1000) / BAUD_RATE;
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
  }

  return 0;
}

// EOF

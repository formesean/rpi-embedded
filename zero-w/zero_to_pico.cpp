#include "../lib/AikaPi/AikaPi.h"

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <string>

#define CS_PIN 16
#define MISO_PIN 19
#define MOSI_PIN 20
#define SCLK_PIN 21
#define BAUD_RATE 1000000

#define PACKET_SIZE 2

int main()
{
  try
  {
    AikaPi &rpi = AikaPi::get_instance();
    rpi.aux.master_enable_spi(0);

    rpi.gpio.set(SCLK_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(MOSI_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::DOWN);
    rpi.gpio.set(MISO_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(CS_PIN, AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::UP);
    rpi.gpio.write(CS_PIN, true);

    // SPI settings
    auto &spi1 = rpi.aux.spi(0);
    spi1.enable();
    spi1.frequency(BAUD_RATE);

    while (true)
    {
      char rx_buffer[PACKET_SIZE] = {0};
      char tx_buffer[PACKET_SIZE] = {0}; // Dummy data

      rpi.gpio.write(CS_PIN, false);
      spi1.xfer(rx_buffer, tx_buffer, PACKET_SIZE);
      rpi.gpio.write(CS_PIN, true);

      uint16_t packet = (static_cast<uint8_t>(rx_buffer[0]) << 8) | static_cast<uint8_t>(rx_buffer[1]);

      std::cout << "Received Packed Value: 0x" << std::hex << std::setw(4) << std::setfill('0') << packet << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}

// EOF

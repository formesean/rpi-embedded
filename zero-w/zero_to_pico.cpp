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

#define PACKET_SIZE 4

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
      char tx_buffer[PACKET_SIZE] = {0x00};
      char rx_buffer[PACKET_SIZE] = {0};

      rpi.gpio.write(CS_PIN, false);

      for (int i = 0; i < PACKET_SIZE; ++i)
      {
        char tx = tx_buffer[i];
        char rx;
        spi1.xfer(&rx, &tx, 1); // send/receive 1 byte
        rx_buffer[i] = rx;
      }

      rpi.gpio.write(CS_PIN, true);

      uint8_t type = rx_buffer[0];
      uint8_t action = rx_buffer[1];
      uint8_t value = rx_buffer[2];
      uint8_t checksum = rx_buffer[3];
      bool valid = ((type ^ action ^ value) == checksum);

      std::cout << "Received Packet: ";
      for (int i = 0; i < PACKET_SIZE; ++i)
      {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << +rx_buffer[i] << " ";
      }

      std::cout << "| Valid: " << std::boolalpha << valid << std::endl;
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

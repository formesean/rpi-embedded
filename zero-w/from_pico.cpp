#include "../lib/AikaPi/AikaPi.h"

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <string>

#define BAUD_RATE 20000000
#define CS_PIN 16
#define MISO_PIN 19
#define MOSI_PIN 20
#define SCLK_PIN 21

#define PACKET_SIZE 2

int main()
{
  try
  {
    AikaPi &rpi = AikaPi::get_instance();
    rpi.aux.master_enable_spi(0);

    rpi.gpio.set(SCLK_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(MISO_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(MOSI_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(CS_PIN, AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::UP);
    rpi.gpio.write(CS_PIN, true);

    auto &spi1 = rpi.aux.spi(0);
    spi1.enable();
    spi1.frequency(BAUD_RATE);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    while (true)
    {
      try
      {
        uint8_t rx_buffer[PACKET_SIZE] = {0};
        uint8_t tx_buffer[PACKET_SIZE] = {0x00, 0x00};

        rpi.gpio.write(CS_PIN, false);
        std::this_thread::sleep_for(std::chrono::microseconds(50));

        spi1.xfer(reinterpret_cast<char *>(rx_buffer),
                  reinterpret_cast<char *>(tx_buffer),
                  PACKET_SIZE);

        std::this_thread::sleep_for(std::chrono::microseconds(50));
        rpi.gpio.write(CS_PIN, true);

        uint16_t packet = (static_cast<uint16_t>(rx_buffer[0]) << 8) |
                          static_cast<uint16_t>(rx_buffer[1]);

        std::cout << "Received: 0x"
                  << std::hex << std::setw(4) << std::setfill('0') << packet;

        std::cout << " (0x" << std::setw(2) << static_cast<int>(rx_buffer[0])
                  << " 0x" << std::setw(2) << static_cast<int>(rx_buffer[1]) << ")";

        if (packet != 0x0000 && packet != 0xFFFF)
        {
          uint8_t high_byte = rx_buffer[0];
          uint8_t low_byte = rx_buffer[1];

          uint8_t type = (high_byte >> 4) & 0x0F;
          uint8_t action = high_byte & 0x0F;
          uint8_t value = (low_byte >> 4) & 0x0F;
          uint8_t checksum = low_byte & 0x0F;

          uint8_t expected_checksum = type ^ action ^ value;

          std::cout << " -> Values: [" << std::hex
                    << "0x" << static_cast<int>(type) << " "
                    << "0x" << static_cast<int>(action) << " "
                    << "0x" << static_cast<int>(value) << " "
                    << "0x" << static_cast<int>(checksum) << "]";

          if (checksum == expected_checksum)
          {
            std::cout << " ✓ VALID";
          }
          else
          {
            std::cout << " ✗ CHECKSUM ERROR (expected 0x"
                      << static_cast<int>(expected_checksum) << ")";
          }
        }
        else
        {
          std::cout << " (no valid data)";
        }

        std::cout << std::dec << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
      }
      catch (const std::exception &inner_e)
      {
        std::cerr << "Transaction error: " << inner_e.what() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      }
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << "Initialization error: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}

// EOF

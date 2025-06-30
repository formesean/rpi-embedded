#include "../lib/AikaPi/AikaPi.h"

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>


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
    rpi.spi.reg(AP::SPI::DLEN, 32);

    rpi.gpio.set(SCLK_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(MISO_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(MOSI_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(CS_PIN, AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::UP);
    rpi.gpio.write(CS_PIN, true);

    auto &spi1 = rpi.aux.spi(0);
    spi1.enable();
    spi1.frequency(BAUD_RATE);
    spi1.shift_length(16);
    spi1.shift_out_ms_bit_first(true);
    spi1.shift_in_ms_bit_first(true);
    spi1.mode(AP::SPI::MODE::_0);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << "Starting SPI communication with Pico..." << std::endl;
    while (true)
    {
      try
      {
        uint16_t rx_data = 0;
        uint16_t tx_data = 0x0000;

        char rx_buffer[2];
        char tx_buffer[2];

        tx_buffer[0] = (tx_data >> 8) & 0xFF;
        tx_buffer[1] = tx_data & 0xFF;

        rpi.gpio.write(CS_PIN, false);
        std::this_thread::sleep_for(std::chrono::microseconds(5));

        spi1.xfer(rx_buffer, tx_buffer, 2);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        rpi.gpio.write(CS_PIN, true);

        rx_data = (static_cast<uint16_t>(static_cast<uint8_t>(rx_buffer[0])) << 8) |
                          static_cast<uint16_t>(static_cast<uint8_t>(rx_buffer[1]));

        if (rx_data != 0x0000 && rx_data != 0xFFFF)
        {
          std::cout << "Packet: 0x" << std::hex << std::setw(4)
                    << std::setfill('0') << rx_data;

          uint8_t high_byte = static_cast<uint8_t>(rx_buffer[0]);
          uint8_t low_byte = static_cast<uint8_t>(rx_buffer[1]);

          uint8_t type = (high_byte >> 4) & 0x0F;
          uint8_t action = high_byte & 0x0F;
          uint8_t value = (low_byte >> 4) & 0x0F;
          uint8_t checksum = low_byte & 0x0F;
          uint8_t expected_checksum = type ^ action ^ value;

          std::cout << " [" << std::hex
                    << "Type=0x" << static_cast<int>(type) << " "
                    << "Action=0x" << static_cast<int>(action) << " "
                    << "Value=0x" << static_cast<int>(value) << " "
                    << "Checksum=0x" << static_cast<int>(checksum) << "]";

          if (checksum == expected_checksum)
            std::cout << " ✓ VALID" << std::endl;
          else
            std::cout << " ✗ INVALID (expected 0x"
                      << static_cast<int>(expected_checksum) << ")" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      } catch (const std::exception &e)
      {
        std::cerr << "SPI error: " << e.what() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

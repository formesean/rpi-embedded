#include "../lib/AikaPi/AikaPi.h"

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <string>

constexpr uint32_t SPI_BAUD = 1000000;
constexpr uint8_t PIN_SCK = 10;
constexpr uint8_t PIN_MISO = 11;
constexpr uint8_t PIN_MOSI = 12;
constexpr uint8_t PIN_CS = 13;

constexpr size_t PACKET_SIZE = 4;

int main()
{
  try
  {
    AikaPi &rpi = AikaPi::get_instance();
    rpi.aux.master_enable_spi(0);

    rpi.gpio.set(PIN_SCK, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(PIN_MISO, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(PIN_MOSI, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::DOWN);
    rpi.gpio.set(PIN_CS, AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::UP);
    rpi.gpio.write(PIN_CS, true);

    auto &spi1 = rpi.aux.spi(0);

    // SPI settings
    spi1.enable();
    spi1.frequency(SPI_BAUD);

    while (true)
    {
      char tx_buffer[PACKET_SIZE] = {0x00};
      char rx_buffer[PACKET_SIZE] = {0};

      rpi.gpio.write(PIN_CS, false);
      spi1.xfer(rx_buffer, tx_buffer, PACKET_SIZE);
      rpi.gpio.write(PIN_CS, true);

      uint8_t type = static_cast<uint8_t>(rx_buffer[0]);
      uint8_t action = static_cast<uint8_t>(rx_buffer[1]);
      uint8_t value = static_cast<uint8_t>(rx_buffer[2]);
      uint8_t checksum = static_cast<uint8_t>(rx_buffer[3]);

      bool valid_checksum = (checksum == (type ^ action ^ value));

      std::cout << "Packet (T): 0x"
                << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(type) << " 0x"
                << std::setw(2) << static_cast<int>(action) << " 0x"
                << std::setw(2) << static_cast<int>(value) << " 0x"
                << std::setw(2) << static_cast<int>(checksum) << " "
                << (valid_checksum ? "true" : "false") << std::endl;

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}

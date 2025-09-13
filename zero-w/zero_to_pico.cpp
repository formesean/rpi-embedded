#include "../lib/AikaPi/AikaPi.h"

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

#define BAUD_RATE 1000000
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
        uint8_t tx_buffer[PACKET_SIZE] = { 0x12, 0x34 };

        spi1.cs(2);
        spi1.xfer(reinterpret_cast<char *>(rx_buffer),
                  reinterpret_cast<char *>(tx_buffer),
                  PACKET_SIZE);

        std::cout << "[LOGAN SPI] TX 16-bit: ";
        std::cout << std::uppercase << std::hex << std::setfill('0');
        uint16_t txw = (static_cast<uint16_t>(tx_buffer[0]) << 8) | tx_buffer[1];
        std::cout << std::setw(4) << txw;
        std::cout << std::dec << std::nouppercase << std::endl;
        uint16_t rxw = (static_cast<uint16_t>(rx_buffer[0]) << 8) | rx_buffer[1];
        std::cout << "[LOGAN SPI] RX 16-bit: " << std::uppercase << std::hex << std::setfill('0')
                  << std::setw(4) << rxw << std::dec << std::nouppercase << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
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

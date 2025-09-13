#include "../lib/AikaPi/AikaPi.h"

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

#define PACKET_SIZE 24

constexpr uint32_t SPI_BAUD = 1000000;
constexpr uint8_t PIN_SCK = 21;
constexpr uint8_t PIN_MISO = 19;
constexpr uint8_t PIN_MOSI = 20;
constexpr uint8_t PIN_CS = 18;


int main()
{
  try
  {
    AikaPi &rpi = AikaPi::get_instance();
    rpi.aux.master_enable_spi(0);

    rpi.gpio.set(PIN_SCK, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(PIN_MISO, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(PIN_MOSI, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    rpi.gpio.set(PIN_CS, AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::UP);
    rpi.gpio.write(PIN_CS, true);

    auto &spi1 = rpi.aux.spi(0);
    spi1.enable();
    spi1.mode(AP::SPI::MODE::_0);
    spi1.shift_length(8);
    spi1.shift_out_ms_bit_first(true);
    spi1.shift_in_ms_bit_first(true);
    spi1.frequency(SPI_BAUD);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    while (true)
    {
      try
      {
        uint8_t rx_buffer[PACKET_SIZE] = {0};
        uint8_t tx_buffer[PACKET_SIZE] = {
          0x4C, 0x43, 0x46, 0x47, 0x01, 0x01, 0x00, 0x04,
          0xD0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x48, 0x42,
          0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        // Use library-controlled CE0 toggling during xfer
        spi1.cs(0); // CE0 -> GPIO18
        spi1.xfer(reinterpret_cast<char *>(rx_buffer),
                  reinterpret_cast<char *>(tx_buffer),
                  PACKET_SIZE);

        std::cout << "[LOGAN SPI] frame (24 bytes): ";
        std::cout << std::uppercase << std::hex << std::setfill('0');
        for (size_t i = 0; i < PACKET_SIZE; ++i)
        {
          std::cout << std::setw(2) << static_cast<int>(tx_buffer[i]);
          if (i + 1 < PACKET_SIZE) std::cout << " ";
        }
        std::cout << std::dec << std::nouppercase << std::endl;
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

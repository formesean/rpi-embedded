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

#define BUFFER_SIZE 17

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

    auto &spi1 = rpi.aux.spi(0);

    // SPI settings
    spi1.enable();
    spi1.frequency(BAUD_RATE);

    // Persistent message buffer
    std::string accumulatedMessage;

    while (true)
    {
      char tx_buffer[BUFFER_SIZE] = {0x00};
      char rx_buffer[BUFFER_SIZE] = {0};

      rpi.gpio.write(CS_PIN, false);
      spi1.xfer(rx_buffer, tx_buffer, BUFFER_SIZE);
      rpi.gpio.write(CS_PIN, true);

      for (int i = 0; i < BUFFER_SIZE; i++)
      {
        if (rx_buffer[i] == '\0')
          break;
        if (rx_buffer[i] >= 32 && rx_buffer[i] <= 126)
          accumulatedMessage += rx_buffer[i];
      }

      std::cout << "Received Message: " << accumulatedMessage << std::endl;
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

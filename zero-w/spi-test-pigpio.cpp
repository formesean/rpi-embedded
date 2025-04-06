#include <iostream>
#include <iomanip>

#include <pigpio.h>

#define SPI_CHANNEL 2
#define SPI_BAUD 1000000 // Adjust as needed
#define SPI_MODE 0
#define SPI_FLAG ((SPI_MODE << 14) | 0x100)
#define CS_PIN 16
#define MISO_PIN 19
#define MOSI_PIN 20
#define SCLK_PIN 21

#define BUFFER_SIZE 17

int main()
{
  if (gpioInitialise() < 0)
  {
    throw std::runtime_error("pigpio initialization failed!");
  }

  int spi_handle = spiOpen(SPI_CHANNEL, SPI_BAUD, SPI_FLAG);

  if (spi_handle < 0)
  {
    gpioTerminate();
    throw std::runtime_error("Failed to open SPI!");
  }

  std::string accumulatedMessage;

  while (true)
  {
    char tx_buffer[BUFFER_SIZE];
    char rx_buffer[BUFFER_SIZE] = {0};

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
      tx_buffer[i] = static_cast<char>(i);
    }

    int result = spiXfer(spi_handle, tx_buffer, rx_buffer, BUFFER_SIZE);

    if (result >= 0)
    {

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
  }

  spiClose(spi_handle);
  gpioTerminate();
  return 0;
}

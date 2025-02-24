#include <iostream>
#include <iomanip>

#include <pigpio.h>

#define SPI_CHANNEL 2
#define SPI_BAUD 250000 // Adjust as needed
#define SPI_MODE 0
#define SPI_FLAG ((SPI_MODE << 14) | 0x100)
#define CS_PIN 16
#define MISO_PIN 19
#define MOSI_PIN 20
#define SCLK_PIN 21

#define BUFFER_SIZE 17

// 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
//  b  b  b  b  b  b  R  T  n  n  n  n  W  A u2 u1 u0 p2 p1 p0  m  m

//  01 0000 0000 0001 0000 0000

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

  std::string accumulatedMessage; // Persistent buffer

  while (true)
  {
    char tx_buffer[BUFFER_SIZE] = {0x00}; // Send dummy bytes
    char rx_buffer[BUFFER_SIZE] = {0};    // Receive buffer

    int result = spiXfer(spi_handle, tx_buffer, rx_buffer, BUFFER_SIZE);
    if (result >= 0)
    {
      // Append received characters to the accumulated message
      for (int i = 0; i < BUFFER_SIZE; i++)
      {
        if (rx_buffer[i] == '\0')
          break;

        if (rx_buffer[i] >= 32 && rx_buffer[i] <= 126)
          accumulatedMessage += rx_buffer[i];
      }

      // Print accumulated message
      std::cout << "Received Message: " << accumulatedMessage << std::endl;
    }
    else
    {
      std::cerr << "SPI transfer failed!" << std::endl;
    }

    gpioDelay(500000);
  }

  spiClose(spi_handle);
  gpioTerminate();
  return 0;
}

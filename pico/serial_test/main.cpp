#include <pico/stdlib.h>
#include <hardware/spi.h>
#include <cstdint>
#include <stdio.h>
#include <string.h>

int main()
{
  stdio_init_all();

  while (!stdio_usb_connected())
    sleep_ms(100);

  while (true)
  {
    printf("Hello from Pico!\n");
    sleep_ms(1000);
  }

  return 0;
}
